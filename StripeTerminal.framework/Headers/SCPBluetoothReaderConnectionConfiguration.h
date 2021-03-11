//
//  SCPBluetoothReaderConnectionConfiguration.h
//  StripeTerminal
//
//  Created by Catriona Scott on 8/6/19.
//  Copyright © 2019 Stripe. All rights reserved.
//
//  Use of this SDK is subject to the Stripe Terminal Terms:
//  https://stripe.com/terminal/legal
//

#import <Foundation/Foundation.h>

#import <StripeTerminal/SCPConnectionConfiguration.h>

NS_ASSUME_NONNULL_BEGIN

/**
 This class lets you set options that define bluetooth reader behavior throughout a given
 reader-to-SDK connection session.
 */

NS_SWIFT_NAME(BluetoothReaderConnectionConfiguration)
@interface SCPBluetoothReaderConnectionConfiguration : SCPConnectionConfiguration

/**
 The ID of the location which the reader should be registered to during connect.

 If nil, the location the reader is registered to will not be changed.

 If the provided id matches the location the reader is already registered to, the
 location will not be changed.

 Note that this parameter is not used for simulated readers.

 @see https://stripe.com/docs/terminal/readers/fleet-management#bbpos-wisepad3-discovery
 */
@property (nonatomic, nullable, readonly) NSString *registerToLocationId;

/**
 Initialize your connect options with a location ID.
 */
- (instancetype)initWithRegisterToLocationId:(NSString *)locationId;

@end

NS_ASSUME_NONNULL_END
