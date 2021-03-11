//
//  ReadReusableCardViewController.swift
//  AppStore
//
//  Created by Ben Guo on 9/12/18.
//  Copyright © 2018 Stripe. All rights reserved.
//

import UIKit
import Static
import StripeTerminal

class ReadReusableCardViewController: TableViewController, TerminalDelegate, BluetoothReaderDelegate, CancelableViewController {

    

    private let headerView = ReaderHeaderView()
    private let logHeaderView = ActivityIndicatorHeaderView(title: "EVENT LOG")
    internal weak var cancelButton: UIBarButtonItem?
    private weak var doneButton: UIBarButtonItem?
    private var completed = false

    internal var cancelable: Cancelable? = nil {
        didSet {
            setAllowedCancelMethods(cancelable != nil ? .all : [])
        }
    }
    private var events: [LogEvent] = [] {
        didSet {
            updateContent()
        }
    }

    init() {
        super.init(style: .grouped)
        self.title = "Testing"
    }

    required init?(coder aDecoder: NSCoder) {
        fatalError("init(coder:) has not been implemented")
    }

    deinit {
        TerminalDelegateAnnouncer.shared.removeListener(self)
    }

    override func viewDidLoad() {
        super.viewDidLoad()
        self.addKeyboardDisplayObservers()

        let doneButton = UIBarButtonItem(barButtonSystemItem: .done, target: self, action: #selector(doneAction))
        doneButton.isEnabled = false
        self.doneButton = doneButton
        let cancelButton = UIBarButtonItem(title: "Cancel", style: .plain, target: self, action: #selector(cancelAction))
        self.cancelButton = cancelButton
        setAllowedCancelMethods([])
        navigationItem.leftBarButtonItem = cancelButton
        navigationItem.rightBarButtonItem = doneButton

        TerminalDelegateAnnouncer.shared.addListener(self, self)
        if completed || Terminal.shared.paymentStatus != .ready {
            return
        }

        headerView.connectedReader = Terminal.shared.connectedReader
        headerView.connectionStatus = Terminal.shared.connectionStatus
        updateContent()

        let params = ReadReusableCardParameters()
        params.metadata = [
            // optional: attach metadata (specific to your app/backend) to the PaymentMethod
            "unique_id": "764BA46C-478D-4EC8-9906-0949B49A60B9"
        ]
        var readEvent = LogEvent(method: .readReusableCard)
        self.events.append(readEvent)
        self.cancelable = Terminal.shared.readReusableCard(params) { paymentMethod, error in
            self.cancelable = nil
            if let error = error {
                readEvent.result = .errored
                readEvent.object = .error(error as NSError)
                self.events.append(readEvent)
                self.complete()
            } else if let paymentMethod = paymentMethod {
                readEvent.result = .succeeded
                readEvent.object = .paymentMethod(paymentMethod)
                self.events.append(readEvent)

                var attachEvent = LogEvent(method: .attachPaymentMethod)
                self.events.append(attachEvent)

                // At this point, you have a `PaymentMethod` of type `card`.
                // This example app sends it to the example-terminal-backend, which attaches it to a
                // customer and returns the attached PaymentMethod with an expanded Customer.
                AppDelegate.apiClient?.attachPaymentMethod(paymentMethod.stripeId, completion: { (json, error) in
                    if let error = error {
                        attachEvent.result = .errored
                        attachEvent.object = .error(error as NSError)
                    } else if let json = json {
                        attachEvent.result = .succeeded
                        attachEvent.object = .json(json)
                    }
                    self.events.append(attachEvent)
                    self.complete()
                })
            }
        }
    }

    private func complete() {
        title = "Test completed"
        doneButton?.isEnabled = true
        logHeaderView.activityIndicator.stopAnimating()
        completed = true
        setAllowedCancelMethods([.swipe])
    }

    private func updateContent() {
        dataSource.sections = [
            Section(header: "", rows: [], footer: Section.Extremity.view(headerView)),
            Section(header: Section.Extremity.view(logHeaderView), rows: events.map { event in
                switch event.result {
                case .started:
                    return Row(text: event.method.rawValue,
                               cellClass: MethodStartCell.self)
                default:
                    return Row(text: event.description, detailText: event.method.rawValue, selection: { [unowned self] in
                        let vc = LogEventViewController(event: event)
                        self.navigationController?.pushViewController(vc, animated: true)
                        }, accessory: .disclosureIndicator,
                           cellClass: LogEventCell.self)
                }
            })
        ]
    }

    @objc
    func doneAction() {
        dismiss(animated: true, completion: nil)
    }

    @objc
    func cancelAction() {
        // cancel collectPaymentMethod
        // todo: rename
        var event = LogEvent(method: .cancelReadReusableCard)
        self.events.append(event)
        cancelable?.cancel { error in
            if let error = error {
                event.result = .errored
                event.object = .error(error as NSError)
            } else {
                event.result = .succeeded
            }
            self.events.append(event)
        }
    }

    // MARK: BluetoothReaderDelegate
    func terminal(_ terminal: Terminal, didChangeConnectionStatus status: ConnectionStatus) {
        headerView.connectionStatus = status
    }

    func terminal(_ terminal: Terminal, didReportUnexpectedReaderDisconnect reader: Reader) {
        presentAlert(title: "Reader disconnected!", message: "\(reader.serialNumber)")
    }

    // MARK: ReaderDisplayDelegate
    
    func reader(_ reader: Reader, didReportAvailableUpdate update: ReaderSoftwareUpdate) {
        
    }
    
    func reader(_ reader: Reader, didStartInstallingUpdate update: ReaderSoftwareUpdate, cancelable: Cancelable) {
        
    }
    
    func reader(_ reader: Reader, didReportReaderSoftwareUpdateProgress progress: Float) {
        
    }
    
    func reader(_ reader: Reader, didFinishInstallingUpdate update: ReaderSoftwareUpdate?, error: Error?) {
        
    }
    
    func reader(_ reader: Reader, didRequestReaderInput inputOptions: ReaderInputOptions = []) {
        var event = LogEvent(method: .requestReaderInput)
        event.result = .message(Terminal.stringFromReaderInputOptions(inputOptions))
        events.append(event)
    }
    
    func reader(_ reader: Reader, didRequestReaderDisplayMessage displayMessage: ReaderDisplayMessage) {
        var event = LogEvent(method: .requestReaderDisplayMessage)
        event.result = .message(Terminal.stringFromReaderDisplayMessage(displayMessage))
        events.append(event)
    }

}
