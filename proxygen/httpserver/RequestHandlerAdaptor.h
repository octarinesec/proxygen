/*
 *  Copyright (c) 2017, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 *
 */
#pragma once

#include <proxygen/httpserver/ResponseHandler.h>
#include <proxygen/lib/http/session/HTTPTransaction.h>

namespace proxygen {

class RequestHandler;
class PushHandler;

/**
 * An adaptor that converts HTTPTransactionHandler to RequestHandler.
 * Apart from that it also -
 *
 * - Handles all the error cases itself as described below. It makes a terminal
 *   call onError(...) where you are expected to log something and stop
 *   processing the request if you have started so.
 *
 *   onError - Send a direct response back if no response has started and
 *             writing is still possible. Otherwise sends an abort.
 *
 * - Handles 100-continue case for you (by sending Continue response)
 */
class RequestHandlerAdaptor
    : public HTTPTransactionHandler,
      public ResponseHandler {
 public:
  explicit RequestHandlerAdaptor(RequestHandler* requestHandler);
  HTTPTransaction* getTransaction() { return txn_;}

 private:
  // HTTPTransactionHandler
  void setTransaction(HTTPTransaction* txn) noexcept override;
  void detachTransaction() noexcept override;
  void onHeadersComplete(std::unique_ptr<HTTPMessage> msg) noexcept override;
  void onBody(std::unique_ptr<folly::IOBuf> chain) noexcept override;
  void onChunkHeader(size_t length) noexcept override;
  void onChunkComplete() noexcept override;
  void onTrailers(std::unique_ptr<HTTPHeaders> trailers) noexcept override;
  void onEOM() noexcept override;
  void onUpgrade(UpgradeProtocol protocol) noexcept override;
  void onError(const HTTPException& error) noexcept override;
  void onEgressPaused() noexcept override;
  void onEgressResumed() noexcept override;

  // ResponseHandler
  void sendHeaders(HTTPMessage& msg) noexcept override;
  void sendChunkHeader(size_t len) noexcept override;
  void sendBody(std::unique_ptr<folly::IOBuf> body) noexcept override;
  void sendChunkTerminator() noexcept override;
  void sendEOM() noexcept override;
  void sendAbort() noexcept override;
  void refreshTimeout() noexcept override;
  void pauseIngress() noexcept override;
  void resumeIngress() noexcept override;
  ResponseHandler* newPushedResponse(
    PushHandler* pushHandler) noexcept override;
  const wangle::TransportInfo& getSetupTransportInfo() const noexcept override;
  void getCurrentTransportInfo(wangle::TransportInfo* tinfo) const override;

  // Helper method
  void setError(ProxygenError err) noexcept;

  HTTPTransaction* txn_{nullptr};
  ProxygenError err_{kErrorNone};
  bool responseStarted_{false};
};

}
