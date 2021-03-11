//
//  SCPDiscoveryConfiguration.h
//  StripeTerminal
//
//  Created by Ben Guo on 7/17/18.
//  Copyright © 2018 Stripe. All rights reserved.
//
//  Use of this SDK is subject to the Stripe Terminal Terms:
//  https://stripe.com/terminal/legal
//

#import <Foundation/Foundation.h>

#import <StripeTerminal/SCPDeviceType.h>
#import <StripeTerminal/SCPDiscoveryMethod.h>

NS_ASSUME_NONNULL_BEGIN

/**
 Configuration options for discovering readers.

 Objects of this type get passed into the `Terminal.shared.discoverReaders()`
 method to control which devices get discovered, and how.

 @see https://stripe.com/docs/terminal/readers/connecting
 */
NS_SWIFT_NAME(DiscoveryConfiguration)
@interface SCPDiscoveryConfiguration : NSObject

/**
 Initializes a discovery configuration with the given discovery method. All discoverable readers
 for that method will be discovered.

 @param discoveryMethod     The discovery method to use.
 @param simulated           Whether to use simulated discovery to discover a
 device simulator. The SDK comes with the ability to simulate behavior without
 using physical hardware. This makes it easy to quickly test your integration
 end-to-end, from pairing with a reader to taking payments.
*/
- (instancetype)initWithDiscoveryMethod:(SCPDiscoveryMethod)discoveryMethod
                              simulated:(BOOL)simulated;

/**
Initializes a discovery configuration with the given discovery method. All discoverable readers
for that method will be discovered.

@param discoveryMethod    The discovery method to use.
@param locationId         The optional location ID to filter the discovered
list to only readers at the specific location. Currently updating and creating
locations is not supported by this SDK and will need to take place on the
backend. Only available for the Internet discovery method.
@param simulated           Whether to use simulated discovery to discover a
device simulator. The SDK comes with the ability to simulate behavior without
using physical hardware. This makes it easy to quickly test your integration
end-to-end, from pairing with a reader to taking payments.
*/
- (instancetype)initWithDiscoveryMethod:(SCPDiscoveryMethod)discoveryMethod
                             locationId:(nullable NSString *)locationId
                              simulated:(BOOL)simulated;

/**
 The timeout (in seconds) after which `discoverReaders:` should fail. If the
 value is `0` (the default), discoverReaders will never timeout.

 Note that setting a timeout is not currently supported when using
 `SCPDiscoveryMethodBluetoothProximity`, or any simulated discovery. If you set
 a timeout when using these configurations, the timeout will be ignored.
 */
@property (nonatomic, assign, readwrite) NSUInteger timeout;

/**
 The method by which to discover readers.
 */
@property (nonatomic, readonly) SCPDiscoveryMethod discoveryMethod;

/**
 Whether to use simulated discovery to discover a device simulator.

 The Terminal SDK comes with the ability to simulate behavior without using
 physical hardware. This makes it easy to quickly test your integration
 end-to-end, from pairing with a reader to taking payments.
 */
@property (nonatomic, readonly) BOOL simulated;

/**
 A location ID that can be used to filter discovery result so only readers
 registered to that location are returned. Currently this is only applicable
 to VerifoneP400 readers.
 */
@property (nonatomic, copy, nullable, readonly) NSString *locationId;

/**
 Use `initWithDiscoveryMethod:simulated:`
 */
- (instancetype)init NS_UNAVAILABLE;

/**
 Use `initWithDiscoveryMethod:simulated:`
 */
+ (instancetype)new NS_UNAVAILABLE;

@end

NS_ASSUME_NONNULL_END
