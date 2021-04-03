//
//  TerminalDelegateAnnouncer.swift
//  Example
//
//  Created by Brian Cooke on 4/24/20.
//  Copyright © 2020 Stripe. All rights reserved.
//

import Foundation
import StripeTerminal

/**
 Allows the Example app to use a single persistent TerminalDelegate and still have the view controllers receive
 the TerminalDelegate events.
 */
class TerminalDelegateAnnouncer: NSObject, TerminalDelegate, BluetoothReaderDelegate {
  
    
    static let shared = TerminalDelegateAnnouncer()

    private var listeners = [WeakBox<TerminalDelegate>]()
    
    private var listeners2 = [WeakBox<BluetoothReaderDelegate>]()

    func addListener(_ delegate: TerminalDelegate, _ delegate2: BluetoothReaderDelegate) {
        if listeners.contains(where: { $0.unbox === delegate }) {
            return
        }
        listeners.append(WeakBox(delegate))
        if listeners2.contains(where: { $0.unbox === delegate2 }) {
            return
        }
        listeners2.append(WeakBox(delegate2))
    }

    func removeListener(_ delegate: TerminalDelegate) {
        // Remove the requested delegate if found and clear out any zero'd references while we're here filtering
        listeners = listeners.filter { $0.unbox !== delegate && $0.unbox != nil }
    }

    // MARK: - TerminalDelegate
    func terminal(_ terminal: Terminal, didReportUnexpectedReaderDisconnect reader: Reader) {
        announce { delegate in
            delegate.terminal(terminal, didReportUnexpectedReaderDisconnect: reader)
        }
    }

    func terminal(_ terminal: Terminal, didChangePaymentStatus status: PaymentStatus) {
        announce { delegate in
            delegate.terminal?(terminal, didChangePaymentStatus: status)
        }
    }

    func terminal(_ terminal: Terminal, didChangeConnectionStatus status: ConnectionStatus) {
        announce { delegate in
            delegate.terminal?(terminal, didChangeConnectionStatus: status)
        }
    }

    // MARK: - Internal

    private func announce(_ announcement: (TerminalDelegate) -> Void) {
        var zerodRefs = [WeakBox<TerminalDelegate>]()

        for weakListener in listeners {
            guard let listener = weakListener.unbox else {
                zerodRefs.append(weakListener)
                continue
            }
            announcement(listener)
        }

        // Remove any zero'd references
        listeners = listeners.filter { listener in
            !zerodRefs.contains(where: { zerod in zerod === listener })
        }
    }
    
    
    private func announce2(_ announcement: (BluetoothReaderDelegate) -> Void) {
        var zerodRefs = [WeakBox<BluetoothReaderDelegate>]()

        for weakListener in listeners2 {
            guard let listener = weakListener.unbox else {
                zerodRefs.append(weakListener)
                continue
            }
            announcement(listener)
        }

        // Remove any zero'd references
        listeners2 = listeners2.filter { listener in
            !zerodRefs.contains(where: { zerod in zerod === listener })
        }
    }
    //BluetoothReaderDelegate
    func reader(_ reader: Reader, didReportAvailableUpdate update: ReaderSoftwareUpdate) {
        announce2 { delegate in
            delegate.reader(reader, didReportAvailableUpdate: update)
        }
    }
    
    func reader(_ reader: Reader, didStartInstallingUpdate update: ReaderSoftwareUpdate, cancelable: Cancelable?) {
        announce2 { delegate in
            delegate.reader(reader, didStartInstallingUpdate: update, cancelable: cancelable)
        }
    }
    
    func reader(_ reader: Reader, didReportReaderSoftwareUpdateProgress progress: Float) {
        announce2 { delegate in
            delegate.reader(reader, didReportReaderSoftwareUpdateProgress: progress)
        }
    }
    
    func reader(_ reader: Reader, didFinishInstallingUpdate update: ReaderSoftwareUpdate?, error: Error?) {
        announce2 { delegate in
            delegate.reader(reader, didFinishInstallingUpdate: update, error: error)
        }
    }
    
    func reader(_ reader: Reader, didRequestReaderInput inputOptions: ReaderInputOptions = []) {
        announce2 { delegate in
            delegate.reader(reader, didRequestReaderInput: inputOptions)
        }
    }
    
    func reader(_ reader: Reader, didRequestReaderDisplayMessage displayMessage: ReaderDisplayMessage) {
        announce2 { delegate in
            delegate.reader(reader, didRequestReaderDisplayMessage: displayMessage)
        }
    }
    
    func reader(_ reader: Reader, didReportReaderEvent event: ReaderEvent, info: [AnyHashable : Any]?) {
            announce2 { delegate in
                delegate.reader?(reader, didReportReaderEvent: event, info: info)
            }
    }

    func readerDidReportLowBatteryWarning(_ reader: Reader) {
        announce2 { delegate in
            delegate.readerDidReportLowBatteryWarning?(reader)
        }
    }
    
}

final class WeakBox<A: AnyObject> {
    weak var unbox: A?
    init(_ value: A) {
        unbox = value
    }
}
