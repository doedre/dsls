#include <fcntl.h>
#include <errno.h>
#include <ifaddrs.h>
#include <machine/apmvar.h>
#include <net/if.h>
#include <net/if_media.h>
#include <net80211/ieee80211.h>
#include <sys/select.h> /* before <sys/ieee80211_ioctl.h> for NBBY */
#include <net80211/ieee80211_ioctl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/param.h>
#include <sys/sched.h>
#include <sys/sysctl.h>
#include <time.h>
#include <unistd.h>

#include "config.h"
#include "util.h"

char essid[MAX_ELEM_LEN];
char ubuf[5];

const char *
separator(const char *sep)
{
	return sep;
}

const char *
null(void)
{
	return NULL;
}

const char *
datetime(const char *fmt)
{
	time_t t;

	t = time(NULL);
	if (!strftime(buf, MAX_ELEM_LEN, fmt, localtime(&t))) {
		return NULL;
	}

	return buf;
}

const char *
battery(void)
{
	int fd;
	struct apm_power_info apm;

	fd = open("/dev/apm", O_RDONLY);
	if (fd < 0) {
		return NULL;
	}

	memset(&apm, 0, sizeof(apm));
	if (ioctl(fd, APM_IOC_GETPOWER, &apm) < 0) {
		close(fd);
		return NULL;
	}
	close(fd);

	switch (apm.battery_state)
	{
	case APM_BATT_HIGH:
		snprintf(buf, MAX_ELEM_LEN, "%3.d", apm.battery_life);
		break;
	case APM_BATT_LOW:
		snprintf(buf, MAX_ELEM_LEN, "%3.d", apm.battery_life);
		break;
	case APM_BATT_CRITICAL:
		snprintf(buf, MAX_ELEM_LEN, "%3.d", apm.battery_life);
		break;
	case APM_BATT_CHARGING:
		snprintf(buf, MAX_ELEM_LEN, "%3.d", apm.battery_life);
		break;
	default:
		snprintf(buf, MAX_ELEM_LEN, "%3.d", apm.battery_life);
		break;
	}
	return buf;
}

const char *
cpu(void)
{
	int mib[2];
	static uintmax_t a[CPUSTATES];
	uintmax_t b[CPUSTATES], sum;
	size_t size;

	mib[0] = CTL_KERN;
	mib[1] = KERN_CPTIME;

	size = sizeof(a);

	memcpy(b, a, sizeof(b));
	if (sysctl(mib, 2, &a, &size, NULL, 0) < 0) {
		return NULL;
	}
	if (b[0] == 0) {
		return NULL;
	}

	sum = (a[CP_USER] + a[CP_NICE] + a[CP_SYS] + a[CP_INTR] + a[CP_IDLE]) -
	      (b[CP_USER] + b[CP_NICE] + b[CP_SYS] + b[CP_INTR] + b[CP_IDLE]);

	if (sum == 0) {
		return NULL;
	}

	snprintf(buf, MAX_ELEM_LEN, "%llu",  100 *
		       ((a[CP_USER] + a[CP_NICE] + a[CP_SYS] +
			 a[CP_INTR]) -
			(b[CP_USER] + b[CP_NICE] + b[CP_SYS] +
			 b[CP_INTR])) / sum);

	return buf;
}

const char *
ram(void)
{
	struct uvmexp uvmexp;
	int uvmexp_mib[] = {CTL_VM, VM_UVMEXP};
	size_t size;

	size = sizeof(uvmexp);

	if (sysctl(uvmexp_mib, 2, &uvmexp, &size, NULL, 0) < 0)
		return NULL;

	return bfmt_human(buf,
			(size_t)(uvmexp.active << (uvmexp.pageshift - 10)) * 1024,
			1000);
}

static int
is_wireless(const char *ifname)
{
	int sockfd;
	if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		return -1;
	}

	struct ifmediareq ifmr;
	memset(&ifmr, 0, sizeof(ifmr));
	strlcpy(ifmr.ifm_name, ifname, IFNAMSIZ);
	if (ioctl(sockfd, SIOCGIFMEDIA, &ifmr) == -1) {
		close(sockfd);
		return -1;
	}

	if (ifmr.ifm_count == 0) {
		close(sockfd);
		return -1;
	}

	uint64_t media_list[ifmr.ifm_count];
	ifmr.ifm_ulist = (uint64_t*)media_list;

	if (ioctl(sockfd, SIOCGIFMEDIA, &ifmr) == -1) {
		close(sockfd);
		free(media_list);
		return -1;
	}

	close(sockfd);

	return IFM_TYPE(ifmr.ifm_current) == IFM_IEEE80211;
}
static int
load_ieee80211_nodereq(const char *interface, struct ieee80211_nodereq *nr)
{
	struct ieee80211_bssid bssid;
	int sockfd;
	uint8_t zero_bssid[IEEE80211_ADDR_LEN];

	memset(&bssid, 0, sizeof(bssid));
	memset(nr, 0, sizeof(struct ieee80211_nodereq));
	if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		return 0;
	}
	strlcpy(bssid.i_name, interface, sizeof(bssid.i_name));
	if ((ioctl(sockfd, SIOCG80211BSSID, &bssid)) < 0) {
		close(sockfd);
		return 0;
	}
	memset(&zero_bssid, 0, sizeof(zero_bssid));
	if (memcmp(bssid.i_bssid, zero_bssid,
	    IEEE80211_ADDR_LEN) == 0) {
		close(sockfd);
		return 0;
	}
	strlcpy(nr->nr_ifname, interface, sizeof(nr->nr_ifname));
	memcpy(&nr->nr_macaddr, bssid.i_bssid, sizeof(nr->nr_macaddr));
	if ((ioctl(sockfd, SIOCG80211NODE, nr)) < 0 && nr->nr_rssi) {
		close(sockfd);
		return 0;
	}

	return close(sockfd), 1;
}

const char *
netstat(void)
{
	struct ifaddrs *ifal, *ifa;
	struct if_data *ifd;
	int wifi;
	uintmax_t oldrxbytes;
	uintmax_t oldtxbytes;
	static uintmax_t rxbytes;
	static uintmax_t txbytes;
	extern const uint64_t wait;
	int if_ok = 0;

	oldrxbytes = rxbytes;
	oldtxbytes = txbytes;

	if (getifaddrs(&ifal) == -1) {
		return NULL;
	}

	rxbytes = 0;
	txbytes = 0;
	for (ifa = ifal; ifa; ifa = ifa->ifa_next) {
		// Skip loopback
		if (!strncmp(ifa->ifa_name, "lo0", IFNAMSIZ))
			continue;


		if ((ifd = (struct if_data *)ifa->ifa_data)) {
			if (ifd->ifi_link_state == LINK_STATE_FULL_DUPLEX) {
				rxbytes += ifd->ifi_ibytes;
				txbytes += ifd->ifi_obytes;
				if_ok = 1;
			} else {
				continue;
			}
		}

		wifi = is_wireless(ifa->ifa_name);
		if (wifi == 1) {
			struct ieee80211_nodereq nr;
			if (load_ieee80211_nodereq(ifa->ifa_name, &nr)) {
				snprintf(essid, MAX_ELEM_LEN, "%s", nr.nr_nwid);
			}
		}

		char rxbuf[10];
		char txbuf[10];
		strncpy(&rxbuf[0], fmt_human((rxbytes - oldrxbytes) * 1000000 / wait,
				          1000), 10);
		strncpy(&txbuf[0], fmt_human((txbytes - oldtxbytes) * 1000000 / wait,
				          1000), 10);
		snprintf(buf, MAX_ELEM_LEN, "%s %s%5s%5s",
				wifi ? "" : "",
				wifi ? essid : "",
				rxbuf, txbuf);
		break;
	}
	freeifaddrs(ifal);
	if (!if_ok) {
		return NULL;
	}
	if ((oldrxbytes == 0) || (oldtxbytes == 0)) {
		return NULL;
	}

	return buf;
}
