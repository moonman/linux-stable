#ifndef NETDEV_PCS_H
#define NETDEV_PCS_H

#include <linux/phy.h>
#include <linux/spinlock.h>
#include <linux/workqueue.h>

struct device_node;
struct ethtool_cmd;
struct net_device;

enum {
	MLO_PAUSE_NONE,
	MLO_PAUSE_ASYM = BIT(0),
	MLO_PAUSE_SYM = BIT(1),
	MLO_PAUSE_RX = BIT(2),
	MLO_PAUSE_TX = BIT(3),
	MLO_PAUSE_TXRX_MASK = MLO_PAUSE_TX | MLO_PAUSE_RX,
	MLO_PAUSE_AN = BIT(4),

	MLO_AN_PHY = 0,
	MLO_AN_FIXED,
	MLO_AN_SGMII,
	MLO_AN_8023Z,
};

struct phylink_link_state {
	u32 supported;
	u32 advertising;
	u32 lp_advertising;
	int speed;
	int duplex;
	int pause;
	unsigned int link:1;
	unsigned int sync:1;
	unsigned int an_enabled:1;
	unsigned int an_complete:1;
};

struct phylink_mac_ops {
	/* Get the ethtool supported mask for the indicated mode */
	int (*mac_get_support)(struct net_device *, unsigned int mode,
			       struct phylink_link_state *);

	/* Read the current link state from the hardware */
	int (*mac_link_state)(struct net_device *, struct phylink_link_state *);

	/* Configure the MAC */
	void (*mac_config)(struct net_device *, unsigned int mode,
			   const struct phylink_link_state *);
	void (*mac_an_restart)(struct net_device *, unsigned int mode);

	void (*mac_link_down)(struct net_device *, unsigned int mode);
	void (*mac_link_up)(struct net_device *, unsigned int mode,
			    struct phy_device *);
};

struct phylink_module_ops {
	int (*get_module_info)(void *, struct ethtool_modinfo *);
	int (*get_module_eeprom)(void *, struct ethtool_eeprom *, u8 *);
};

struct phylink *phylink_create(struct net_device *, struct device_node *,
	phy_interface_t iface, const struct phylink_mac_ops *ops);
void phylink_destroy(struct phylink *);

int phylink_connect_phy(struct phylink *, struct phy_device *);
int phylink_of_phy_connect(struct phylink *, struct device_node *);
void phylink_disconnect_phy(struct phylink *);

void phylink_mac_change(struct phylink *, bool up);

void phylink_start(struct phylink *);
void phylink_stop(struct phylink *);

int phylink_ethtool_get_settings(struct phylink *, struct ethtool_cmd *);
int phylink_ethtool_set_settings(struct phylink *, struct ethtool_cmd *);
int phylink_ethtool_nway_reset(struct phylink *);
void phylink_ethtool_get_pauseparam(struct phylink *,
				    struct ethtool_pauseparam *);
int phylink_ethtool_set_pauseparam(struct phylink *,
				   struct ethtool_pauseparam *);
int phylink_ethtool_get_module_info(struct phylink *, struct ethtool_modinfo *);
int phylink_ethtool_get_module_eeprom(struct phylink *,
				      struct ethtool_eeprom *, u8 *);
int phylink_init_eee(struct phylink *, bool);
int phylink_get_eee_err(struct phylink *);
int phylink_ethtool_get_eee(struct phylink *, struct ethtool_eee *);
int phylink_ethtool_set_eee(struct phylink *, struct ethtool_eee *);
int phylink_mii_ioctl(struct phylink *, struct ifreq *, int);

int phylink_register_module(struct phylink *, void *,
			    const struct phylink_module_ops *);
int phylink_unregister_module(struct phylink *, void *);

void phylink_set_link_port(struct phylink *pl, u32 support, u8 port);
int phylink_set_link_an_mode(struct phylink *pl, unsigned int mode);
void phylink_disable(struct phylink *pl);
void phylink_enable(struct phylink *pl);
struct phylink *phylink_lookup_by_netdev(struct net_device *ndev);

#endif
