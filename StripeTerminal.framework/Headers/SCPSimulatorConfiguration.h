//
//  SCPSimulatorConfiguration.h
//  StripeTerminal
//
//  Created by Brian Cooke on 5/29/20.
//  Copyright © 2020 Stripe. All rights reserved.
//
//  Use of this SDK is subject to the Stripe Terminal Terms:
//  https://stripe.com/terminal/legal
//

#import <Foundation/Foundation.h>

#import <StripeTerminal/SCPSimulateReaderUpdate.h>

NS_ASSUME_NONNULL_BEGIN

/**
 Simulator specific configurations you can set to test your integration.

 Set the properties via `[SCPTerminal shared].simulatorConfiguration.`
 */
NS_SWIFT_NAME(SimulatorConfiguration)
@interface SCPSimulatorConfiguration : NSObject

/**
 Set this to different values of the SCPSimulateReaderUpdate enum to
 test different reader software update states of your integration.

 This is only valid for simulated bluetooth readers.
 */
@property (nonatomic, assign) SCPSimulateReaderUpdate availableReaderUpdate;

/**
 You cannot directly instantiate this class.
 Set exposed properties via `[SCPTerminal shared].simulatorConfiguration`
 */
- (instancetype)init NS_UNAVAILABLE;

/**
 You cannot directly instantiate this class.
 Set exposed properties via `[SCPTerminal shared].simulatorConfiguration`
 */
+ (instancetype)new NS_UNAVAILABLE;

@end

NS_ASSUME_NONNULL_END
