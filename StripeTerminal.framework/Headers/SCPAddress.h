//
//  SCPAddress.h
//  StripeTerminal
//
//  Created by Brian Cooke on 6/22/20.
//  Copyright © 2020 Stripe. All rights reserved.
//
//  Use of this SDK is subject to the Stripe Terminal Terms:
//  https://stripe.com/terminal/legal
//

#import <Foundation/Foundation.h>

#import <StripeTerminal/SCPJSONDecodable.h>

NS_ASSUME_NONNULL_BEGIN

/**
 Stores address data for use with the `SCPLocation` class.

 @see https://stripe.com/docs/api/terminal/locations/object#terminal_location_object-address
 @see `SCPLocation`
 */
NS_SWIFT_NAME(Address)
@interface SCPAddress : NSObject <SCPJSONDecodable>

/**
 The city name
 */
@property (nonatomic, nullable, copy, readonly) NSString *city;

/**
 The country code
 */
@property (nonatomic, nullable, copy, readonly) NSString *country;

/**
 The first line of the address
 */
@property (nonatomic, nullable, copy, readonly) NSString *line1;

/**
 The second line of the address
 */
@property (nonatomic, nullable, copy, readonly) NSString *line2;

/**
 The postal code of the address
 */
@property (nonatomic, nullable, copy, readonly) NSString *postalCode;

/**
 The state of the address
 */
@property (nonatomic, nullable, copy, readonly) NSString *state;

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
