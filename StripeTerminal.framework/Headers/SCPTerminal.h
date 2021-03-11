//
//  SCPTerminal.h
//  StripeTerminal
//
//  Created by Ben Guo on 7/28/17.
//  Copyright © 2017 Stripe. All rights reserved.
//
//  Use of this SDK is subject to the Stripe Terminal Terms:
//  https://stripe.com/terminal/legal
//

#import <Foundation/Foundation.h>

#import <StripeTerminal/SCPBlocks.h>
#import <StripeTerminal/SCPBluetoothReaderDelegate.h>
#import <StripeTerminal/SCPCart.h>
#import <StripeTerminal/SCPConnectionStatus.h>
#import <StripeTerminal/SCPDeviceType.h>
#import <StripeTerminal/SCPDiscoveryMethod.h>
#import <StripeTerminal/SCPLogLevel.h>
#import <StripeTerminal/SCPPaymentStatus.h>
#import <StripeTerminal/SCPReaderEvent.h>
#import <StripeTerminal/SCPRefundParameters.h>
#import <StripeTerminal/SCPSimulatorConfiguration.h>

NS_ASSUME_NONNULL_BEGIN

/**
 The current version of this library.
 */
static NSString *const SCPSDKVersion = @"2.0.0-b5";

@class SCPCancelable,
    SCPBluetoothReaderConnectionConfiguration,
    SCPDiscoveryConfiguration,
    SCPInternetReaderConnectionConfiguration,
    SCPPaymentIntentParameters,
    SCPReadReusableCardParameters,
    SCPUpdateReaderSoftwareParameters;

@protocol SCPConnectionTokenProvider
, SCPTerminalDelegate, SCPDiscoveryDelegate;

/**
 The `SCPTerminal` object that is made available by the Stripe Terminal SDK exposes
 a generic interface for discovering readers, connecting to a reader, and
 creating payments.

 The `SCPTerminal` object is a singleton, and you cannot create multiple
 `SCPTerminal` instances.
 */
NS_SWIFT_NAME(Terminal)
@interface SCPTerminal : NSObject

/**
 Sets the token provider for the shared (singleton) `SCPTerminal` instance.

 You must set a token provider before calling `shared` to initialize the
 Terminal singleton. We recommend calling `setTokenProvider:` in your
 AppDelegate's `application:didFinishLaunchingWithOptions:` method.
 Alternatively, you can wrap your call to `setTokenProvider:` with a
 `dispatch_once` in Objective-C, or use a static constructor in Swift.

 Note that you may only set a token provider *before* requesting the shared
 Terminal instance for the first time. In order to switch accounts in your app,
 e.g. to switch between live and test Stripe API keys on your backend, refer
 to the documentation for the `clearCachedCredentials` method on the shared
 Terminal instance.
 */
+ (void)setTokenProvider:(id<SCPConnectionTokenProvider>)tokenProvider NS_SWIFT_NAME(setTokenProvider(_:));

/**
 Returns true if a token provider has been set, through `setTokenProvider:`
 */
+ (BOOL)hasTokenProvider;

/**
 Sets a block to listen for logs from the shared Terminal instance (optional).

 You can use this optional method to listen for logs from the Stripe Terminal
 SDK and incorporate them into your own remote logs. Note that these logs
 are subject to change, and provided for informational and debugging purposes
 only. You should not depend on them for behavior in your app. Also note that
 the block you provide may be called from any thread.

 To print internal logs from the SDK to the console, you can set `logLevel` to
 `SCPLogLevelVerbose` on the Terminal instance.
 */
+ (void)setLogListener:(SCPLogListenerBlock)listener;

/**
 Returns the shared (singleton) Terminal instance.

 Before accessing the singleton for the first time, you must first call
 `setTokenProvider:` and `setDelegate:`.
 */
@property (class, nonatomic, readonly) SCPTerminal *shared;

/**
 The Terminal instance's delegate.

 Set this to handle events from the Terminal instance.

 This is optional for Verifone P400 and required for bluetooth readers,
 Chipper 2X BT & WisePad 3.

 A runtime assert will fire if attempting to connect to a reader
 that requires the delegate.
 */
@property (nonatomic, nullable, readwrite) id<SCPTerminalDelegate> delegate;

/**
 Information about the connected `SCPReader`, or nil if no reader is connected.
 */
@property (nonatomic, nullable, readonly) SCPReader *connectedReader;

/**
 The Terminal instance's current connection status.
 */
@property (nonatomic, readonly) SCPConnectionStatus connectionStatus;

/**
 The log level for the SDK. The default value is `.none`.
 */
@property (nonatomic, assign, readwrite) SCPLogLevel logLevel;

/**
 The simulator configuration that will be used when connecting to a simulated reader.
 This object will always exist and has readwrite properties you can configure with
 `Terminal.shared.simulatorConfiguration...` before using a simulated reader.

 @see SCPSimulatorConfiguration
 */
@property (nonatomic, readonly) SCPSimulatorConfiguration *simulatorConfiguration;

/**
 The Terminal instance's current payment status.
 */
@property (nonatomic, readonly) SCPPaymentStatus paymentStatus;

/**
 Clears the current connection token, saved reader sessions, and any other
 cached credentials. You can use this method to switch accounts in your app,
 e.g. to switch between live and test Stripe API keys on your backend.

 In order to switch accounts in your app:
 - if a reader is connected, call `disconnectReader:`
 - configure the `tokenProvider` object to return connection tokens for the new account.
 The `tokenProvider` is implemented by your code, and you can do this however you want.
 - call `clearCachedCredentials`
 - call `discoverReaders` and `connectReader` to connect to a reader. The `connect` call
 will request a new connection token from your backend server via the token provider.

 An overview of the lifecycle of a connection token under the hood:
 - When the Terminal singleton is initialized, the SDK attempts to proactively request
 a connection token from your backend server.
 - When `connect` is called, the SDK uses the connection token and reader
 information to create a reader session.
 - Subsequent calls to `connect` will re-use the reader session if the SDK
 has successfully connected to the reader already. Otherwise, it will require a
 new connection token when you call `connect` again.
 */
- (void)clearCachedCredentials NS_SWIFT_NAME(clearCachedCredentials());

/**
 Begins discovering readers matching the given configuration.

 When `discoverReaders` is called, the terminal begins scanning for readers using
 the settings in the given `SCPDiscoveryConfiguration`. You must implement
 `SCPDiscoveryDelegate` to handle displaying discovery results to your user and
 connecting to a selected reader.

 The discovery process will stop on its own when the terminal successfully
 connects to a reader, if the command is canceled, or if a discovery error occurs.

 Note that if `discoverReaders` is canceled, the completion block will be called
 with `nil` (rather than an `SCPErrorCanceled` error).

 @see https://stripe.com/docs/terminal/readers/connecting

 @param configuration   The configuration for reader discovery.
 @param delegate        Your delegate for reader discovery.
 @param completion      The completion block called when the command completes.
 */
- (nullable SCPCancelable *)discoverReaders:(SCPDiscoveryConfiguration *)configuration
                                   delegate:(id<SCPDiscoveryDelegate>)delegate
                                 completion:(SCPErrorCompletionBlock)completion NS_SWIFT_NAME(discoverReaders(_:delegate:completion:));

/**
 Attempts to connect to the given bluetooth reader with a given connection
 configuration.

 If the connect succeeds, the completion block will be called with the
 connected reader, and `SCPTerminal.connectionStatus` will change to `.connected`.

 If the connect fails, the completion block will be called with an error.

 The SDK must be actively discovering readers in order to connect to one.
 The discovery process will stop if this connection request succeeds, otherwise
 the SDK will continue discovering.

 When this method is called, the SDK uses a connection token and the given
 reader information to create a reader session. If the SDK does not already
 have a connection token, it will call the `fetchConnectionToken method you
 defined to fetch one.

 If `connectionConfig` is set to `nil`, the SDK will resort to
 default connection behavior; see the `SCPBluetoothReaderConnectionConfiguration`
 header documentation for more details.

 If the reader's battery is critically low the connect call will fail with
 `SCPErrorBluetoothDisconnected`. Plug your reader in to start charging and
 try again.

 @see https://stripe.com/docs/terminal/readers/connecting

 @param reader          The reader to connect to. This should be a reader
 recently returned to the `didUpdateDiscoveredReaders:` method.
 @param delegate        The delegate used during the lifetime of the connection.
 See `SCPBluetoothReaderDelegate`.
 @param connectionConfig   The connection configuration for options while
 connecting to a reader. See `SCPConnectionConfiguration` for more details.
 @param completion      The completion block called when the command completes.
 */
- (void)connectBluetoothReader:(SCPReader *)reader
                      delegate:(id<SCPBluetoothReaderDelegate>)delegate
              connectionConfig:(nullable SCPBluetoothReaderConnectionConfiguration *)connectionConfig
                    completion:(SCPReaderCompletionBlock)completion NS_SWIFT_NAME(connectBluetoothReader(_:delegate:connectionConfig:completion:));

/**
 Attempts to connect to the given internet reader with a given connection
 configuration.

 If the connect succeeds, the completion block will be called with the
 connected reader, and `SCPTerminal.connectionStatus` will change to `.connected`.

 If the connect fails, the completion block will be called with an error.

 The SDK must be actively discovering readers in order to connect to one.
 The discovery process will stop if this connection request succeeds, otherwise
 the SDK will continue discovering.

 When this method is called, the SDK uses a connection token and the given
 reader information to create a reader session. If the SDK does not already
 have a connection token, it will call the `fetchConnectionToken method you
 defined to fetch one.

 If `connectionConfig` is set to `nil`, the SDK will resort to
 default connection behavior; see the `SCPInternetReaderConnectionConfiguration`
 header documentation for more details.

 @see https://stripe.com/docs/terminal/readers/connecting

 @param reader          The reader to connect to. This should be a reader
 recently returned to the `didUpdateDiscoveredReaders:` method.
 @param connectionConfig   The connection configuration for options while
 connecting to a reader. See `SCPConnectionConfiguration` for more details.
 @param completion      The completion block called when the command completes.
 */
- (void)connectInternetReader:(SCPReader *)reader
             connectionConfig:(nullable SCPInternetReaderConnectionConfiguration *)connectionConfig
                   completion:(SCPReaderCompletionBlock)completion NS_SWIFT_NAME(connectInternetReader(_:connectionConfig:completion:));

/**
 Attempts to disconnect from the currently connected reader.

 If the disconnect succeeds, the completion block is called with `nil`. If the
 disconnect fails, the completion block is called with an error.

 @see https://stripe.com/docs/terminal/readers/connecting

 @param completion      The completion block called when the command completes.
 */
- (void)disconnectReader:(SCPErrorCompletionBlock)completion NS_SWIFT_NAME(disconnectReader(_:));

/**
 Creates a new `SCPPaymentIntent` with the given parameters.

 Note: If the information required to create a PaymentIntent isn't readily
 available in your app, you can create the PaymentIntent on your server and use
 the `retrievePaymentIntent` method to retrieve the PaymentIntent in your app.

     @note This cannot be used with the Verifone P400 reader.  If used with the
     Verifone P400, the completion handler will be called with
     an SCPErrorFeatureNotAvailableWithConnectedReader error.

 @see https://stripe.com/docs/terminal/payments#create

 @param parameters      The parameters for the PaymentIntent to be created.
 @param completion      The completion block called when the command completes.
 */
- (void)createPaymentIntent:(SCPPaymentIntentParameters *)parameters
                 completion:(SCPPaymentIntentCompletionBlock)completion NS_SWIFT_NAME(createPaymentIntent(_:completion:));

/**
 Retrieves a `SCPPaymentIntent` with a client secret.

 If the information required to create a PaymentIntent isn't readily available
 in your app, you can create the PaymentIntent on your server and use this
 method to retrieve the PaymentIntent in your app.

 @see https://stripe.com/docs/terminal/payments#create

 @param clientSecret    The client secret of the PaymentIntent to be retrieved.
 @param completion      The completion block called when the command completes.
 */
- (void)retrievePaymentIntent:(NSString *)clientSecret
                   completion:(SCPPaymentIntentCompletionBlock)completion NS_SWIFT_NAME(retrievePaymentIntent(clientSecret:completion:));

/**
 Collects a payment method for the given `SCPPaymentIntent`.

 Note: `collectPaymentMethod` does not apply any changes to the PaymentIntent
 API object. Updates to the PaymentIntent are local to the SDK, and persisted
 in-memory.

 If collecting a payment method fails, the completion block will be called with
 an error. After resolving the error, you may call `collectPaymentMethod` again
 to either try the same card again, or try a different card.

 If collecting a payment method succeeds, the completion block will be called
 with a PaymentIntent with status `.requiresConfirmation`, indicating that you
 should call `processPayment:completion:` to finish the payment.

 Note that if `collectPaymentMethod` is canceled, the completion block will be
 called with a `Canceled` error.

 @see https://stripe.com/docs/terminal/payments#collect

 @param paymentIntent       The PaymentIntent to collect a payment method for.
 @param completion          The completion block called when the command completes.
 */
- (nullable SCPCancelable *)collectPaymentMethod:(SCPPaymentIntent *)paymentIntent
                                      completion:(SCPPaymentIntentCompletionBlock)completion NS_SWIFT_NAME(collectPaymentMethod(_:completion:));

/**
 Processes a payment after collecting a payment method succeeds.

 Synchronous capture
 -------------------

 Stripe Terminal uses two-step card payments to prevent unintended and duplicate
 payments. When `processPayment` completes successfully, a charge has been
 authorized on the customer's card, but not yet been "captured". Your app must
 **synchronously notify your backend** to capture the PaymentIntent
 in order to settle the funds to your account.

 Handling failures
 -----------------

 When `processPayment` fails, the SDK returns an error that includes the
 updated `SCPPaymentIntent`. Your app should inspect the updated PaymentIntent
 to decide how to retry the payment.

 1. If the updated PaymentIntent is `nil`, the request to Stripe's servers timed
 out and the PaymentIntent's status is unknown. We recommend that you retry
 `processPayment` with the original PaymentIntent. If you instead choose to
 abandon the original PaymentIntent and create a new one, **do not capture**
 the original PaymentIntent. If you do, you might charge your customer twice.

 2. If the updated PaymentIntent's status is still `.requiresConfirmation` (e.g.,
 the request failed because your app is not connected to the internet), you
 can call `processPayment` again with the updated PaymentIntent to retry
 the request.

 3. If the updated PaymentIntent's status changes to `.requiresPaymentMethod` (e.g., the
 request failed because the card was declined), call `collectPaymentMethod` with the
 updated PaymentIntent to try charging another card.

 @see https://stripe.com/docs/terminal/payments#process

 @param paymentIntent   The PaymentIntent to confirm.
 @param completion      The completion block called when the confirm completes.
 */
- (void)processPayment:(SCPPaymentIntent *)paymentIntent
            completion:(SCPProcessPaymentCompletionBlock)completion NS_SWIFT_NAME(processPayment(_:completion:));

/**
 Cancels an `SCPPaymentIntent`.

 If the cancel request succeeds, the completion block will be called with the
 updated PaymentIntent object with status Canceled. If the cancel request
 fails, the completion block will be called with an error.

 @note This cannot be used with the Verifone P400 reader.  If used with the
 Verifone P400, the completion handler will be called with
 an SCPErrorFeatureNotAvailableWithConnectedReader error.

 @see https://stripe.com/docs/terminal/payments/refunds

 @param paymentIntent     The PaymentIntent to cancel.
 @param completion        The completion block called when the cancel completes.
 */
- (void)cancelPaymentIntent:(SCPPaymentIntent *)paymentIntent
                 completion:(SCPPaymentIntentCompletionBlock)completion NS_SWIFT_NAME(cancelPaymentIntent(_:completion:));

/**
 Reads a card with the given parameters and returns a PaymentMethod.

 **NOTE: Most integrations should not use `readReusableCard`.**

 You should create a `SCPPaymentIntent` and use the associated `collectPaymentMethod`
 and `processPayment` methods if you are simply collecting a payment from
 a customer.

 You can use `readReusableCard` to read payment details and defer payment for later.
 The PaymentMethod created by this method will have type `card`, suitable for
 use with online payments.

 Note that if you use this method to defer a payment, the transaction will
 *not* receive the beneficial rates and liability shift associated with card
 present transactions.

 If reading a card fails, the completion block will be called with an error
 containing details about the failure. If reading a card succeeds, the
 completion block will be called with a `PaymentMethod`. You should send
 the ID of the payment method to your backend for further processing. For example,
 you can use the fingerprint to look up charges created using the same
 card.

 @note This cannot be used with the Verifone P400 or WisePad 3 readers. This
 method will assert if called while the SDK is connected to a device of
 type `verifoneP400` or `wisePad3`.

 @see https://stripe.com/docs/terminal/online-payments

 @param parameters  The parameters for reading the card.
 @param completion  The completion block called when the command completes.
 */
- (nullable SCPCancelable *)readReusableCard:(SCPReadReusableCardParameters *)parameters
                                  completion:(SCPPaymentMethodCompletionBlock)completion NS_SWIFT_NAME(readReusableCard(_:completion:));

/**
 Initiates an in-person refund with a given set of `SCPRefundParameters` by
 collecting the payment method that is to be refunded.

 Some payment methods, like Interac Debit payments, require that in-person payments
 also be refunded while the cardholder is present. The cardholder must present
 the Interac card to the card reader; these payments cannot be refunded via the
 dashboard or the API.

 For payment methods that don't require the cardholder be present, see
 https://stripe.com/docs/terminal/payments/refunds

 This method, along with `processRefund`, allow you to design an in-person refund
 flow into your app.

 If collecting a payment method fails, the completion block will be called with
 an error. After resolving the error, you may call `collectRefundPaymentMethod`
 again to either try the same card again, or try a different card.

 If collecting a payment method succeeds, the completion block will be called
 with an `nil` error. At that point, you can call `processRefund` to finish
 refunding the payment method.

 Calling any other SDK methods between `collectRefundPaymentMethod` and
 `processRefund` will result in undefined behavior.

 Note that if `collectRefundPaymentMethod` is canceled, the completion block
 will be called with a `Canceled` error.

 @see https://stripe.com/docs/terminal/canada#interac-refunds

 @param refundParams  The SCPRefundParameters object that describes how the
 refund will be created.
 @param completion  The completion block called when the command completes.
 */
- (nullable SCPCancelable *)collectRefundPaymentMethod:(SCPRefundParameters *)refundParams
                                            completion:(SCPErrorCompletionBlock)completion
    NS_SWIFT_NAME(collectRefundPaymentMethod(_:completion:));

/**
 Processes an in-person refund after the refund payment method has been collected.

 The completion block will either be called with the successful `SCPRefund` or
 with an `SCPProcessRefundError`.

 When `processRefund` fails, the SDK returns an error that either includes the
 failed `SCPRefund` or the `SCPRefundParameters` that led to a failure.
 Your app should inspect the `SCPProcessRefundError` to decide how to proceed.

 1. If the refund property is `nil`, the request to Stripe's servers timed
 out and the refund's status is unknown. We recommend that you retry
 `processRefund` with the original `SCPRefundParameters`.

 2. If the `SCPProcessRefundError` has a `failure_reason`, the refund was declined.
 We recommend that you take action based on the decline code you received.

 @note `collectRefundPaymentMethod:completion` and `processRefund` are only
 available for payment methods that require in-person refunds. For all other
 refunds, use the Stripe Dashboard or the Stripe API.

 @see https://stripe.com/docs/terminal/canada#interac-refunds

 @param completion  The completion block called when the command completes.
 */
- (void)processRefund:(SCPProcessRefundCompletionBlock)completion NS_SWIFT_NAME(processRefund(completion:));

/**
 Clears the reader display and resets it to the splash screen.

 @param completion  The completion block called when the command completes.

 @note Only available for the Verifone P400
 */
- (void)clearReaderDisplay:(SCPErrorCompletionBlock)completion NS_SWIFT_NAME(clearReaderDisplay(_:));

/**
 Updates the reader display with cart information.
 This method is for display purposes only and has no correlation with what the customer is
 actually charged. Tax and total are also not automatically calculated and must be set in SCPCart.

 @param cart  The cart containing the information that will be displayed.
 @param completion  The completion block called when the command completes.

 @note Only available for the Verifone P400
 */
- (void)setReaderDisplay:(SCPCart *)cart
              completion:(SCPErrorCompletionBlock)completion
    NS_SWIFT_NAME(setReaderDisplay(_:completion:));

/**
 Installs the available update for the connected reader.

 If an error occurs while installing the update (e.g. because the update was
 interrupted), the `SCPBluetoothReaderDelegate` will receive
 `reader:didFinishInstallingUpdate:error:` with the error. If the update
 completed successfully, the same method will be called with `nil` error.
 In your app you should display the progress of the update to the user.
 You should also instruct the user to wait for the update to complete:
 "Do not leave this page, and keep the reader in range and powered on until
 the update is complete."

 You must implement the ability to update your reader's software in your app.
 Though we expect required software updates to be very rare, by using Stripe
 Terminal, you are obligated to include this functionality.

 @note It is an error to call this method when the SDK is connected to the Verifone
 P400 or WisePOS E readers.

 @see https://stripe.com/docs/terminal/readers/bbpos-chipper2xbt#updating-reader-software
 @see https://stripe.com/docs/terminal/readers/bbpos-wisepad3#updating-reader-software
 */
- (void)installAvailableUpdate;

/**
 Returns an unlocalized string for the given reader input options, e.g.
 "Swipe / Insert"
 */
+ (NSString *)stringFromReaderInputOptions:(SCPReaderInputOptions)options NS_SWIFT_NAME(stringFromReaderInputOptions(_:));

/**
 Returns an unlocalized string for the given reader display message, e.g.
 "Retry Card"
 */
+ (NSString *)stringFromReaderDisplayMessage:(SCPReaderDisplayMessage)message NS_SWIFT_NAME(stringFromReaderDisplayMessage(_:));

/**
 Returns an unlocalized string for the given reader event, e.g.
 "Card Inserted"
 */
+ (NSString *)stringFromReaderEvent:(SCPReaderEvent)event NS_SWIFT_NAME(stringFromReaderEvent(_:));

/**
 Returns an unlocalized string for the given connection status, e.g.
 "Connecting"
 */
+ (NSString *)stringFromConnectionStatus:(SCPConnectionStatus)status NS_SWIFT_NAME(stringFromConnectionStatus(_:));

/**
 Returns an unlocalized string for the given payment status, e.g.
 "Not Ready"
 */
+ (NSString *)stringFromPaymentStatus:(SCPPaymentStatus)status NS_SWIFT_NAME(stringFromPaymentStatus(_:));

/**
 Returns an unlocalized string for the given device type.
 */
+ (NSString *)stringFromDeviceType:(SCPDeviceType)deviceType NS_SWIFT_NAME(stringFromDeviceType(_:));

/**
 Returns an unlocalized string for the given discovery method.
 */
+ (NSString *)stringFromDiscoveryMethod:(SCPDiscoveryMethod)method NS_SWIFT_NAME(stringFromDiscoveryMethod(_:));

/**
 Use `initWithConfiguration:tokenProvider:delegate:`
 */
- (instancetype)init NS_UNAVAILABLE;

/**
 Use `initWithConfiguration:tokenProvider:delegate:`
 */
+ (instancetype)new NS_UNAVAILABLE;

@end

NS_ASSUME_NONNULL_END
