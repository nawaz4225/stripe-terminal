//
//  SCPReader.h
//  StripeTerminal
//
//  Created by Ben Guo on 8/1/17.
//  Copyright © 2017 Stripe. All rights reserved.
//
//  Use of this SDK is subject to the Stripe Terminal Terms:
//  https://stripe.com/terminal/legal
//

#import <Foundation/Foundation.h>

#import <StripeTerminal/SCPDeviceType.h>
#import <StripeTerminal/SCPJSONDecodable.h>
#import <StripeTerminal/SCPLocation.h>
#import <StripeTerminal/SCPLocationStatus.h>
#import <StripeTerminal/SCPReaderNetworkStatus.h>

NS_ASSUME_NONNULL_BEGIN

@class SCPReaderSoftwareUpdate;

/**
 Information about a reader that has been discovered or connected to the SDK.
 Note some of the properties are specific to a certain device type and are
 labeled as such. If a property does not call out a specific device then it
 is applicable to all device types.
 
 @see https://stripe.com/docs/terminal/readers
 */
NS_SWIFT_NAME(Reader)
@interface SCPReader : NSObject <SCPJSONDecodable>

/**
 The IP address of the reader. (Verifone P400 only.)
 */
@property (nonatomic, nullable, readonly) NSString *ipAddress;

/**
 The location ID of the reader.

 For readers that have not been registered to a location this may
 be set to your account's default location.
 
 @see https://stripe.com/docs/api/terminal/locations
 */
@property (nonatomic, nullable, readonly) NSString *locationId;

/**
 Used to tell whether the registeredLocation object has been set.
 Note that the Verifone P400 and simulated readers will always
 have an `unknown` `locationStatus`.
 (Chipper 2X BT and WisePad 3 only.)
 */
@property (nonatomic, readonly) SCPLocationStatus locationStatus;

/**
 The location details this reader is registered to, if any.

 During discovery, `registeredLocation` will be nil if the reader is not yet
 registered to your account or if it hasn't been assigned to a location and
 is in the "Ungrouped readers" section on the Stripe Dashboard.

 You can assign a reader to a location from the SDK during `connectReader`.
 See `registerToLocationId` in `SCPConnectionConfiguration`.

 After connecting to a reader, `registeredLocation` will be nil if the reader has been
 registered to a new location. See https://stripe.com/docs/api/terminal/locations/retrieve
 for documentation on retrieving location details in your app.

 Note that the Verifone P400 and simulated readers will always
 have a nil `registeredLocation`.

 (Chipper 2X BT and WisePad 3 only.)
 
 @see https://stripe.com/docs/api/terminal/locations
 @see https://stripe.com/docs/terminal/readers/fleet-management#bbpos-wisepad3
 @see `SCPConnectionConfiguration`
 */
@property (nonatomic, nullable, readonly) SCPLocation *registeredLocation;

/**
 The networking status of the reader: either `offline` or `online`. Note that
 the Chipper 2X and the WisePad 3's statuses will always be `offline`.
 (Verifone P400 only.)
 */
@property (nonatomic, readonly) SCPReaderNetworkStatus status;

/**
 A custom label that may be given to a reader for easier identification.
 (Verifone P400 only.)
 */
@property (nonatomic, nullable, readonly) NSString *label;

/**
 The reader's battery level, represented as a boxed float in the range `[0, 1]`.
 If the reader does not have a battery, or the battery level is unknown, this
 value is `nil`. (Chipper 2X and WisePad 3 only.)
 */
@property (nonatomic, nullable, readonly) NSNumber *batteryLevel;

/**
 The Stripe unique identifier for the reader.
 */
@property (nonatomic, nullable, readonly) NSString *stripeId;

/**
 The reader's device type.
 */
@property (nonatomic, readonly) SCPDeviceType deviceType;

/**
 True if this is a simulated reader.

 `SCPDiscoveryConfiguration` objects with `simulated` set to `true` produce simulated
 readers.
 */
@property (nonatomic, readonly) BOOL simulated;

/**
 The reader's serial number.
 */
@property (nonatomic, readonly) NSString *serialNumber;

/**
 The reader's current device software version, or `nil` if this information is
 unavailable.
 */
@property (nonatomic, nullable, readonly) NSString *deviceSoftwareVersion;

/**
 The available update for this reader, or nil if no update is available.
 This update will also have been announced via
 `- [BluetoothReaderDelegate reader:didReportAvailableUpdate:]`

 Install this update with `- [Terminal installAvailableUpdate]`

 calls to `installAvailableUpdate` when `availableUpdate` is nil will result
 in `- [BluetoothReaderDelegate reader:didFinishInstallingUpdate:error:]` called
 immediately with a nil update and nil error.
 */
@property (nonatomic, nullable, readonly) SCPReaderSoftwareUpdate *availableUpdate;

/**
 You cannot directly instantiate this class.
 */
- (instancetype)init NS_UNAVAILABLE;

/**
 You cannot directly instantiate this class.
 */
+ (instancetype)new NS_UNAVAILABLE;

@end

NS_ASSUME_NONNULL_END
