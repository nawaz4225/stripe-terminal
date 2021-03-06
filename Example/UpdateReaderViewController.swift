//
//  UpdateReaderViewController.swift
//  Example
//
//  Created by Ben Guo on 9/29/18.
//  Copyright © 2018 Stripe. All rights reserved.
//

import UIKit
import Static
import StripeTerminal

class UpdateReaderViewController: TableViewController, TerminalDelegate, BluetoothReaderDelegate, CancelableViewController {
    
    

    private let headerView = ReaderHeaderView()
    private weak var doneButton: UIBarButtonItem?
    internal weak var cancelButton: UIBarButtonItem?

    private var update: ReaderSoftwareUpdate?
    private var updateProgress: Float?
    private var checkForUpdateCancelable: Cancelable? = nil {
        didSet {
            updateContent()
        }
    }
    private var installUpdateCancelable: Cancelable? = nil {
        didSet {
            updateContent()
        }
    }

    init() {
        super.init(style: .grouped)
        self.title = "Update Reader"
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
        navigationItem.leftBarButtonItem = cancelButton
        navigationItem.rightBarButtonItem = doneButton

        TerminalDelegateAnnouncer.shared.addListener(self, self)
        headerView.connectedReader = Terminal.shared.connectedReader
        headerView.connectionStatus = Terminal.shared.connectionStatus
        updateContent()
    }

    private func installUpdate() {
//        guard let update = self.update else { return }
        // Don't allow swiping away during an install to prevent accidentally canceling the update.
        setAllowedCancelMethods([.button])
//        UIApplication.shared.isIdleTimerDisabled = true
        Terminal.shared.installAvailableUpdate()
//        installUpdateCancelable = Terminal.shared.installUpdate(update, delegate: self, completion: { error in
//            UIApplication.shared.isIdleTimerDisabled = false
//            if let e = error {
//                self.presentAlert(error: e)
//            }
//            self.updateProgress = nil
//            if self.installUpdateCancelable != nil {
//                if error == nil {
//                    self.presentAlert(title: "Update successful", message: "The reader may restart at the end of the update. If this happens, reconnect the app to the reader.")
//                    self.doneButton?.isEnabled = true
//                    self.setAllowedCancelMethods([.swipe])
//                    self.update = nil
//                }
//                self.installUpdateCancelable = nil
//            }
//        })
    }

    private func updateContent() {
        let currentVersion = Terminal.shared.connectedReader?.deviceSoftwareVersion ?? "unknown"
        let canInstallUpdate = update != nil
        let updateButtonText: String
        let updateFooter: String
        let updateRow: Row
        if canInstallUpdate {
            guard let update = update else { return }
            let updateVersion = update.deviceSoftwareVersion
            if let progress = updateProgress {
                let percent = Int(progress*100)
                updateFooter = "Update progress: \(percent)%\n\n⚠️ The reader will temporarily become unresponsive. Do not leave this page, and keep the reader in range and powered on until the update is complete."
                updateRow = Row(text: "⏱ Update in progress")
            } else if installUpdateCancelable != nil {
                updateFooter = "Starting to install update..."
                updateRow = Row(text: "Installing Update")
            } else {
                updateButtonText = "Install update"
                let updateEstimate = ReaderSoftwareUpdate.string(from: update.estimatedUpdateTime)
                updateFooter = "Target version:\n\(updateVersion)\n\nThe reader will become unresponsive until the update is complete. Estimated update time: \(updateEstimate)"
                updateRow = Row(text: updateButtonText, selection: { [unowned self] in
                    self.installUpdate()
                    }, cellClass: ButtonCell.self)
            }
        } else {
            updateButtonText = "Check for update"
            updateFooter = "Checking for update..."
            updateRow = Row(text: updateButtonText)
            
        }
        dataSource.sections = [
            Section(header: "", rows: [], footer: Section.Extremity.view(headerView)),
            Section(header: "Current Version", rows: [
                Row(text: currentVersion, cellClass: Value1Cell.self)
            ]),
            Section(header: "", rows: [updateRow], footer: Section.Extremity.title(updateFooter))
        ]
    }

    @objc
    func cancelAction() {
        if let cancelable = cancelable {
            setAllowedCancelMethods([])
            cancelable.cancel { error in
                self.setAllowedCancelMethods(.all)
                if let error = error {
                    self.presentAlert(error: error)
                }
            }
        } else {
            dismiss(animated: true, completion: nil)
        }
    }

    @objc
    func doneAction() {
        dismiss(animated: true, completion: nil)
    }

    // MARK: TerminalDelegate
    func terminal(_ terminal: Terminal, didChangeConnectionStatus status: ConnectionStatus) {
        headerView.connectionStatus = status
    }

    func terminal(_ terminal: Terminal, didReportUnexpectedReaderDisconnect reader: Reader) {
        presentAlert(title: "Reader disconnected!", message: "\(reader.serialNumber)") { _ in
            self.dismiss(animated: true, completion: nil)
        }
        headerView.connectedReader = nil
    }

    // MARK: ReaderSoftwareUpdateDelegate
    
    func reader(_ reader: Reader, didReportAvailableUpdate update: ReaderSoftwareUpdate) {
        self.update = update
        self.checkForUpdateCancelable = nil
        updateContent()
    }
    
    func reader(_ reader: Reader, didStartInstallingUpdate update: ReaderSoftwareUpdate, cancelable: Cancelable?) {
        
    }
    
    func reader(_ reader: Reader, didReportReaderSoftwareUpdateProgress progress: Float) {
        
        updateProgress = progress
        updateContent()
    }
    
    func reader(_ reader: Reader, didFinishInstallingUpdate update: ReaderSoftwareUpdate?, error: Error?) {
        
    }
    
    func reader(_ reader: Reader, didRequestReaderInput inputOptions: ReaderInputOptions = []) {
        
    }
    
    func reader(_ reader: Reader, didRequestReaderDisplayMessage displayMessage: ReaderDisplayMessage) {
        
    }

    // MARK: - CancelableViewController

    var cancelable: Cancelable? {
        checkForUpdateCancelable ?? installUpdateCancelable
    }
}
