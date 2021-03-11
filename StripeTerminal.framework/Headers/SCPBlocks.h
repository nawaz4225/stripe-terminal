//
//  SCPBlocks.h
//  StripeTerminal
//
//  Created by Ben Guo on 7/28/17.
//  Copyright © 2017 Stripe. All rights reserved.
//
//  Use of this SDK is subject to the Stripe Terminal Terms:
//  https://stripe.com/terminal/legal
//

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

@class SCPPaymentMethod,
    SCPProcessPaymentError,
    SCPProcessRefundError,
    SCPReader,
    SCPReaderSoftwareUpdate,
    SCPPaymentIntent,
    SCPRefundParameters,
    SCPRefund;

/**
 A block called with a connection token or an error from your backend.
 @see SCPConnectionTokenProvider

 @param token       The connection token from your backend server.
 @param error       An error if one occurred, or nil.
 */
typedef void (^SCPConnectionTokenCompletionBlock)(NSString *__nullable token, NSError *__nullable error)
    NS_SWIFT_NAME(ConnectionTokenCompletionBlock);

/**
 A block called with a logline from the SDK.

 @param logline     An internal logline from the SDK.
 */
typedef void (^SCPLogListenerBlock)(NSString *_Nonnull logline)
    NS_SWIFT_NAME(LogListenerBlock);

/**
 A block called with a PaymentMethod.

 @param paymentMethod  A PaymentMethod object, or nil if an error occurred.
 @param error          An error if one occurred, or nil.
 */
typedef void (^SCPPaymentMethodCompletionBlock)(SCPPaymentMethod *__nullable paymentMethod, NSError *__nullable error)
    NS_SWIFT_NAME(PaymentMethodCompletionBlock);

/**
 A block called with an optional error.

 @param error       The error, or nil if no error occured.
 */
typedef void (^SCPErrorCompletionBlock)(NSError *__nullable error)
    NS_SWIFT_NAME(ErrorCompletionBlock);

/**
 A block called with a PaymentIntent or a ProcessPaymentError

 @param intent      The PaymentIntent, or nil.
 @param error       An error if one occurred, or nil.
 */
typedef void (^SCPProcessPaymentCompletionBlock)(SCPPaymentIntent *__nullable intent, SCPProcessPaymentError *__nullable error)
    NS_SWIFT_NAME(ProcessPaymentCompletionBlock);

/**
 A block called with a Refund or a ProcessRefundError.
 
 @param refund      The Refund, or nil.
 @param error       An error if one occurred, or nil.
 */
typedef void (^SCPProcessRefundCompletionBlock)(SCPRefund *__nullable refund, SCPProcessRefundError *__nullable error)
    NS_SWIFT_NAME(ProcessRefundCompletionBlock);

/**
 A block called with a PaymentIntent or an error.

 @param intent      The PaymentIntent, or nil.
 @param error       An error if one occurred, or nil.
 */
typedef void (^SCPPaymentIntentCompletionBlock)(SCPPaymentIntent *__nullable intent, NSError *__nullable error)
    NS_SWIFT_NAME(PaymentIntentCompletionBlock);

/**
 A block called with a reader object or an error.

 @param reader      A reader object, or nil.
 @param error       An error if one occurred, or nil.
 */
typedef void (^SCPReaderCompletionBlock)(SCPReader *__nullable reader, NSError *__nullable error)
    NS_SWIFT_NAME(ReaderCompletionBlock);

NS_ASSUME_NONNULL_END
