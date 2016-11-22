/*
 * hidl interface for wpa_supplicant daemon
 * Copyright (c) 2004-2016, Jouni Malinen <j@w1.fi>
 * Copyright (c) 2004-2016, Roshan Pius <rpius@google.com>
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */

#ifndef WPA_SUPPLICANT_HIDL_STA_IFACE_H
#define WPA_SUPPLICANT_HIDL_STA_IFACE_H

#include <array>
#include <vector>

#include <android-base/macros.h>

#include <android/hardware/wifi/supplicant/1.0/ISupplicantStaIface.h>
#include <android/hardware/wifi/supplicant/1.0/ISupplicantStaIfaceCallback.h>
#include <android/hardware/wifi/supplicant/1.0/ISupplicantStaNetwork.h>

extern "C" {
#include "utils/common.h"
#include "utils/includes.h"
#include "wpa_supplicant_i.h"
#include "config.h"
#include "driver_i.h"
#include "wpa.h"
}

namespace android {
namespace hardware {
namespace wifi {
namespace supplicant {
namespace V1_0 {
namespace implementation {

/**
 * Implementation of StaIface hidl object. Each unique hidl
 * object is used for control operations on a specific interface
 * controlled by wpa_supplicant.
 */
class StaIface
    : public android::hardware::wifi::supplicant::V1_0::ISupplicantStaIface
{
public:
	StaIface(struct wpa_global* wpa_global, const char ifname[]);
	~StaIface() override = default;
	// HIDL does not provide a built-in mechanism to let the server
	// invalidate a HIDL interface object after creation. If any client
	// process holds onto a reference to the object in their context,
	// any method calls on that reference will continue to be directed to
	// the server.
	// However Supplicant HAL needs to control the lifetime of these
	// objects. So, add a public |invalidate| method to all |Iface| and
	// |Network| objects.
	// This will be used to mark an object invalid when the corresponding
	// iface or network is removed.
	// All HIDL method implementations should check if the object is still
	// marked valid before processing them.
	void invalidate();
	bool isValid();

	// Hidl methods exposed.
	Return<void> getName(getName_cb _hidl_cb) override;
	Return<void> getType(getType_cb _hidl_cb) override;
	Return<void> addNetwork(addNetwork_cb _hidl_cb) override;
	Return<void> removeNetwork(
	    SupplicantNetworkId id, removeNetwork_cb _hidl_cb) override;
	Return<void> getNetwork(
	    SupplicantNetworkId id, getNetwork_cb _hidl_cb) override;
	Return<void> listNetworks(listNetworks_cb _hidl_cb) override;
	Return<void> registerCallback(
	    const sp<ISupplicantStaIfaceCallback>& callback,
	    registerCallback_cb _hidl_cb) override;
	Return<void> reassociate(reassociate_cb _hidl_cb) override;
	Return<void> reconnect(reconnect_cb _hidl_cb) override;
	Return<void> disconnect(disconnect_cb _hidl_cb) override;
	Return<void> setPowerSave(
	    bool enable, setPowerSave_cb _hidl_cb) override;
	Return<void> initiateTdlsDiscover(
	    const hidl_array<uint8_t, 6>& mac_address,
	    initiateTdlsDiscover_cb _hidl_cb) override;
	Return<void> initiateTdlsSetup(
	    const hidl_array<uint8_t, 6>& mac_address,
	    initiateTdlsSetup_cb _hidl_cb) override;
	Return<void> initiateTdlsTeardown(
	    const hidl_array<uint8_t, 6>& mac_address,
	    initiateTdlsTeardown_cb _hidl_cb) override;
	Return<void> initiateAnqpQuery(
	    const hidl_array<uint8_t, 6>& mac_address,
	    const hidl_vec<ISupplicantStaIface::AnqpInfoId>& info_elements,
	    const hidl_vec<ISupplicantStaIface::Hs20AnqpSubtypes>& sub_types,
	    initiateAnqpQuery_cb _hidl_cb) override;
	Return<void> initiateHs20IconQuery(
	    const hidl_array<uint8_t, 6>& mac_address,
	    const hidl_string& file_name,
	    initiateHs20IconQuery_cb _hidl_cb) override;

private:
	// Corresponding worker functions for the HIDL methods.
	std::pair<SupplicantStatus, std::string> getNameInternal();
	std::pair<SupplicantStatus, IfaceType> getTypeInternal();
	std::pair<SupplicantStatus, sp<ISupplicantNetwork>>
	addNetworkInternal();
	SupplicantStatus removeNetworkInternal(SupplicantNetworkId id);
	std::pair<SupplicantStatus, sp<ISupplicantNetwork>> getNetworkInternal(
	    SupplicantNetworkId id);
	std::pair<SupplicantStatus, std::vector<SupplicantNetworkId>>
	listNetworksInternal();
	SupplicantStatus registerCallbackInternal(
	    const sp<ISupplicantStaIfaceCallback>& callback);
	SupplicantStatus reassociateInternal();
	SupplicantStatus reconnectInternal();
	SupplicantStatus disconnectInternal();
	SupplicantStatus setPowerSaveInternal(bool enable);
	SupplicantStatus initiateTdlsDiscoverInternal(
	    const std::array<uint8_t, 6>& mac_address);
	SupplicantStatus initiateTdlsSetupInternal(
	    const std::array<uint8_t, 6>& mac_address);
	SupplicantStatus initiateTdlsTeardownInternal(
	    const std::array<uint8_t, 6>& mac_address);
	SupplicantStatus initiateAnqpQueryInternal(
	    const std::array<uint8_t, 6>& mac_address,
	    const std::vector<ISupplicantStaIface::AnqpInfoId>& info_elements,
	    const std::vector<ISupplicantStaIface::Hs20AnqpSubtypes>&
		sub_types);
	SupplicantStatus initiateHs20IconQueryInternal(
	    const std::array<uint8_t, 6>& mac_address,
	    const std::string& file_name);

	struct wpa_supplicant* retrieveIfacePtr();

	// Reference to the global wpa_struct. This is assumed to be valid for
	// the lifetime of the process.
	const struct wpa_global* wpa_global_;
	// Name of the iface this hidl object controls
	const std::string ifname_;
	bool is_valid_;

	DISALLOW_COPY_AND_ASSIGN(StaIface);
};

}  // namespace implementation
}  // namespace V1_0
}  // namespace wifi
}  // namespace supplicant
}  // namespace hardware
}  // namespace android

#endif  // WPA_SUPPLICANT_HIDL_STA_IFACE_H