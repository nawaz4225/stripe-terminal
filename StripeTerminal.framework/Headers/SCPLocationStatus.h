//
//  SCPLocationStatus.h
//  StripeTerminal
//
//  Created by Brian Cooke on 6/26/20.
//  Copyright © 2020 Stripe. All rights reserved.
//
//  Use of this SDK is subject to the Stripe Terminal Terms:
//  https://stripe.com/terminal/legal
//

NS_ASSUME_NONNULL_BEGIN

/**
 Represents the possible states of the `registeredLocation` object for a
 discovered reader.

 @see `SCPReader`
 */
typedef NS_ENUM(NSUInteger, SCPLocationStatus) {
    /**
     The location is not known. `registeredLocation` will be nil.
     */
    SCPLocationStatusUnknown,

    /**
     The location was successfully set to a known location.
     `registeredLocation` is a valid `SCPLocation`.
     */
    SCPLocationStatusSet,

    /**
     This location is not set. `registeredLocation` will be nil.
     */
    SCPLocationStatusNotSet,

} NS_SWIFT_NAME(LocationStatus);

NS_ASSUME_NONNULL_END
