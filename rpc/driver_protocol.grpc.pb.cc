// Generated by the gRPC C++ plugin.
// If you make any local change, they will be lost.
// source: driver_protocol.proto

#include "driver_protocol.pb.h"
#include "driver_protocol.grpc.pb.h"

#include <functional>
#include <grpcpp/support/async_stream.h>
#include <grpcpp/support/async_unary_call.h>
#include <grpcpp/impl/channel_interface.h>
#include <grpcpp/impl/client_unary_call.h>
#include <grpcpp/support/client_callback.h>
#include <grpcpp/support/message_allocator.h>
#include <grpcpp/support/method_handler.h>
#include <grpcpp/impl/rpc_service_method.h>
#include <grpcpp/support/server_callback.h>
#include <grpcpp/impl/server_callback_handlers.h>
#include <grpcpp/server_context.h>
#include <grpcpp/impl/service_type.h>
#include <grpcpp/support/sync_stream.h>
namespace rocvad {

static const char* DriverProtocol_method_names[] = {
  "/rocvad.DriverProtocol/ping",
  "/rocvad.DriverProtocol/driver_info",
  "/rocvad.DriverProtocol/stream_logs",
  "/rocvad.DriverProtocol/get_all_devices",
  "/rocvad.DriverProtocol/get_device",
  "/rocvad.DriverProtocol/add_device",
  "/rocvad.DriverProtocol/delete_device",
  "/rocvad.DriverProtocol/bind",
  "/rocvad.DriverProtocol/connect",
};

std::unique_ptr< DriverProtocol::Stub> DriverProtocol::NewStub(const std::shared_ptr< ::grpc::ChannelInterface>& channel, const ::grpc::StubOptions& options) {
  (void)options;
  std::unique_ptr< DriverProtocol::Stub> stub(new DriverProtocol::Stub(channel, options));
  return stub;
}

DriverProtocol::Stub::Stub(const std::shared_ptr< ::grpc::ChannelInterface>& channel, const ::grpc::StubOptions& options)
  : channel_(channel), rpcmethod_ping_(DriverProtocol_method_names[0], options.suffix_for_stats(),::grpc::internal::RpcMethod::NORMAL_RPC, channel)
  , rpcmethod_driver_info_(DriverProtocol_method_names[1], options.suffix_for_stats(),::grpc::internal::RpcMethod::NORMAL_RPC, channel)
  , rpcmethod_stream_logs_(DriverProtocol_method_names[2], options.suffix_for_stats(),::grpc::internal::RpcMethod::SERVER_STREAMING, channel)
  , rpcmethod_get_all_devices_(DriverProtocol_method_names[3], options.suffix_for_stats(),::grpc::internal::RpcMethod::NORMAL_RPC, channel)
  , rpcmethod_get_device_(DriverProtocol_method_names[4], options.suffix_for_stats(),::grpc::internal::RpcMethod::NORMAL_RPC, channel)
  , rpcmethod_add_device_(DriverProtocol_method_names[5], options.suffix_for_stats(),::grpc::internal::RpcMethod::NORMAL_RPC, channel)
  , rpcmethod_delete_device_(DriverProtocol_method_names[6], options.suffix_for_stats(),::grpc::internal::RpcMethod::NORMAL_RPC, channel)
  , rpcmethod_bind_(DriverProtocol_method_names[7], options.suffix_for_stats(),::grpc::internal::RpcMethod::NORMAL_RPC, channel)
  , rpcmethod_connect_(DriverProtocol_method_names[8], options.suffix_for_stats(),::grpc::internal::RpcMethod::NORMAL_RPC, channel)
  {}

::grpc::Status DriverProtocol::Stub::ping(::grpc::ClientContext* context, const ::rocvad::PrNone& request, ::rocvad::PrNone* response) {
  return ::grpc::internal::BlockingUnaryCall< ::rocvad::PrNone, ::rocvad::PrNone, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(channel_.get(), rpcmethod_ping_, context, request, response);
}

void DriverProtocol::Stub::async::ping(::grpc::ClientContext* context, const ::rocvad::PrNone* request, ::rocvad::PrNone* response, std::function<void(::grpc::Status)> f) {
  ::grpc::internal::CallbackUnaryCall< ::rocvad::PrNone, ::rocvad::PrNone, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(stub_->channel_.get(), stub_->rpcmethod_ping_, context, request, response, std::move(f));
}

void DriverProtocol::Stub::async::ping(::grpc::ClientContext* context, const ::rocvad::PrNone* request, ::rocvad::PrNone* response, ::grpc::ClientUnaryReactor* reactor) {
  ::grpc::internal::ClientCallbackUnaryFactory::Create< ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(stub_->channel_.get(), stub_->rpcmethod_ping_, context, request, response, reactor);
}

::grpc::ClientAsyncResponseReader< ::rocvad::PrNone>* DriverProtocol::Stub::PrepareAsyncpingRaw(::grpc::ClientContext* context, const ::rocvad::PrNone& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderHelper::Create< ::rocvad::PrNone, ::rocvad::PrNone, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(channel_.get(), cq, rpcmethod_ping_, context, request);
}

::grpc::ClientAsyncResponseReader< ::rocvad::PrNone>* DriverProtocol::Stub::AsyncpingRaw(::grpc::ClientContext* context, const ::rocvad::PrNone& request, ::grpc::CompletionQueue* cq) {
  auto* result =
    this->PrepareAsyncpingRaw(context, request, cq);
  result->StartCall();
  return result;
}

::grpc::Status DriverProtocol::Stub::driver_info(::grpc::ClientContext* context, const ::rocvad::PrNone& request, ::rocvad::PrDriverInfo* response) {
  return ::grpc::internal::BlockingUnaryCall< ::rocvad::PrNone, ::rocvad::PrDriverInfo, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(channel_.get(), rpcmethod_driver_info_, context, request, response);
}

void DriverProtocol::Stub::async::driver_info(::grpc::ClientContext* context, const ::rocvad::PrNone* request, ::rocvad::PrDriverInfo* response, std::function<void(::grpc::Status)> f) {
  ::grpc::internal::CallbackUnaryCall< ::rocvad::PrNone, ::rocvad::PrDriverInfo, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(stub_->channel_.get(), stub_->rpcmethod_driver_info_, context, request, response, std::move(f));
}

void DriverProtocol::Stub::async::driver_info(::grpc::ClientContext* context, const ::rocvad::PrNone* request, ::rocvad::PrDriverInfo* response, ::grpc::ClientUnaryReactor* reactor) {
  ::grpc::internal::ClientCallbackUnaryFactory::Create< ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(stub_->channel_.get(), stub_->rpcmethod_driver_info_, context, request, response, reactor);
}

::grpc::ClientAsyncResponseReader< ::rocvad::PrDriverInfo>* DriverProtocol::Stub::PrepareAsyncdriver_infoRaw(::grpc::ClientContext* context, const ::rocvad::PrNone& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderHelper::Create< ::rocvad::PrDriverInfo, ::rocvad::PrNone, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(channel_.get(), cq, rpcmethod_driver_info_, context, request);
}

::grpc::ClientAsyncResponseReader< ::rocvad::PrDriverInfo>* DriverProtocol::Stub::Asyncdriver_infoRaw(::grpc::ClientContext* context, const ::rocvad::PrNone& request, ::grpc::CompletionQueue* cq) {
  auto* result =
    this->PrepareAsyncdriver_infoRaw(context, request, cq);
  result->StartCall();
  return result;
}

::grpc::ClientReader< ::rocvad::PrLogEntry>* DriverProtocol::Stub::stream_logsRaw(::grpc::ClientContext* context, const ::rocvad::PrNone& request) {
  return ::grpc::internal::ClientReaderFactory< ::rocvad::PrLogEntry>::Create(channel_.get(), rpcmethod_stream_logs_, context, request);
}

void DriverProtocol::Stub::async::stream_logs(::grpc::ClientContext* context, const ::rocvad::PrNone* request, ::grpc::ClientReadReactor< ::rocvad::PrLogEntry>* reactor) {
  ::grpc::internal::ClientCallbackReaderFactory< ::rocvad::PrLogEntry>::Create(stub_->channel_.get(), stub_->rpcmethod_stream_logs_, context, request, reactor);
}

::grpc::ClientAsyncReader< ::rocvad::PrLogEntry>* DriverProtocol::Stub::Asyncstream_logsRaw(::grpc::ClientContext* context, const ::rocvad::PrNone& request, ::grpc::CompletionQueue* cq, void* tag) {
  return ::grpc::internal::ClientAsyncReaderFactory< ::rocvad::PrLogEntry>::Create(channel_.get(), cq, rpcmethod_stream_logs_, context, request, true, tag);
}

::grpc::ClientAsyncReader< ::rocvad::PrLogEntry>* DriverProtocol::Stub::PrepareAsyncstream_logsRaw(::grpc::ClientContext* context, const ::rocvad::PrNone& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncReaderFactory< ::rocvad::PrLogEntry>::Create(channel_.get(), cq, rpcmethod_stream_logs_, context, request, false, nullptr);
}

::grpc::Status DriverProtocol::Stub::get_all_devices(::grpc::ClientContext* context, const ::rocvad::PrNone& request, ::rocvad::PrDeviceList* response) {
  return ::grpc::internal::BlockingUnaryCall< ::rocvad::PrNone, ::rocvad::PrDeviceList, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(channel_.get(), rpcmethod_get_all_devices_, context, request, response);
}

void DriverProtocol::Stub::async::get_all_devices(::grpc::ClientContext* context, const ::rocvad::PrNone* request, ::rocvad::PrDeviceList* response, std::function<void(::grpc::Status)> f) {
  ::grpc::internal::CallbackUnaryCall< ::rocvad::PrNone, ::rocvad::PrDeviceList, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(stub_->channel_.get(), stub_->rpcmethod_get_all_devices_, context, request, response, std::move(f));
}

void DriverProtocol::Stub::async::get_all_devices(::grpc::ClientContext* context, const ::rocvad::PrNone* request, ::rocvad::PrDeviceList* response, ::grpc::ClientUnaryReactor* reactor) {
  ::grpc::internal::ClientCallbackUnaryFactory::Create< ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(stub_->channel_.get(), stub_->rpcmethod_get_all_devices_, context, request, response, reactor);
}

::grpc::ClientAsyncResponseReader< ::rocvad::PrDeviceList>* DriverProtocol::Stub::PrepareAsyncget_all_devicesRaw(::grpc::ClientContext* context, const ::rocvad::PrNone& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderHelper::Create< ::rocvad::PrDeviceList, ::rocvad::PrNone, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(channel_.get(), cq, rpcmethod_get_all_devices_, context, request);
}

::grpc::ClientAsyncResponseReader< ::rocvad::PrDeviceList>* DriverProtocol::Stub::Asyncget_all_devicesRaw(::grpc::ClientContext* context, const ::rocvad::PrNone& request, ::grpc::CompletionQueue* cq) {
  auto* result =
    this->PrepareAsyncget_all_devicesRaw(context, request, cq);
  result->StartCall();
  return result;
}

::grpc::Status DriverProtocol::Stub::get_device(::grpc::ClientContext* context, const ::rocvad::PrDeviceSelector& request, ::rocvad::PrDeviceInfo* response) {
  return ::grpc::internal::BlockingUnaryCall< ::rocvad::PrDeviceSelector, ::rocvad::PrDeviceInfo, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(channel_.get(), rpcmethod_get_device_, context, request, response);
}

void DriverProtocol::Stub::async::get_device(::grpc::ClientContext* context, const ::rocvad::PrDeviceSelector* request, ::rocvad::PrDeviceInfo* response, std::function<void(::grpc::Status)> f) {
  ::grpc::internal::CallbackUnaryCall< ::rocvad::PrDeviceSelector, ::rocvad::PrDeviceInfo, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(stub_->channel_.get(), stub_->rpcmethod_get_device_, context, request, response, std::move(f));
}

void DriverProtocol::Stub::async::get_device(::grpc::ClientContext* context, const ::rocvad::PrDeviceSelector* request, ::rocvad::PrDeviceInfo* response, ::grpc::ClientUnaryReactor* reactor) {
  ::grpc::internal::ClientCallbackUnaryFactory::Create< ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(stub_->channel_.get(), stub_->rpcmethod_get_device_, context, request, response, reactor);
}

::grpc::ClientAsyncResponseReader< ::rocvad::PrDeviceInfo>* DriverProtocol::Stub::PrepareAsyncget_deviceRaw(::grpc::ClientContext* context, const ::rocvad::PrDeviceSelector& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderHelper::Create< ::rocvad::PrDeviceInfo, ::rocvad::PrDeviceSelector, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(channel_.get(), cq, rpcmethod_get_device_, context, request);
}

::grpc::ClientAsyncResponseReader< ::rocvad::PrDeviceInfo>* DriverProtocol::Stub::Asyncget_deviceRaw(::grpc::ClientContext* context, const ::rocvad::PrDeviceSelector& request, ::grpc::CompletionQueue* cq) {
  auto* result =
    this->PrepareAsyncget_deviceRaw(context, request, cq);
  result->StartCall();
  return result;
}

::grpc::Status DriverProtocol::Stub::add_device(::grpc::ClientContext* context, const ::rocvad::PrDeviceInfo& request, ::rocvad::PrDeviceInfo* response) {
  return ::grpc::internal::BlockingUnaryCall< ::rocvad::PrDeviceInfo, ::rocvad::PrDeviceInfo, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(channel_.get(), rpcmethod_add_device_, context, request, response);
}

void DriverProtocol::Stub::async::add_device(::grpc::ClientContext* context, const ::rocvad::PrDeviceInfo* request, ::rocvad::PrDeviceInfo* response, std::function<void(::grpc::Status)> f) {
  ::grpc::internal::CallbackUnaryCall< ::rocvad::PrDeviceInfo, ::rocvad::PrDeviceInfo, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(stub_->channel_.get(), stub_->rpcmethod_add_device_, context, request, response, std::move(f));
}

void DriverProtocol::Stub::async::add_device(::grpc::ClientContext* context, const ::rocvad::PrDeviceInfo* request, ::rocvad::PrDeviceInfo* response, ::grpc::ClientUnaryReactor* reactor) {
  ::grpc::internal::ClientCallbackUnaryFactory::Create< ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(stub_->channel_.get(), stub_->rpcmethod_add_device_, context, request, response, reactor);
}

::grpc::ClientAsyncResponseReader< ::rocvad::PrDeviceInfo>* DriverProtocol::Stub::PrepareAsyncadd_deviceRaw(::grpc::ClientContext* context, const ::rocvad::PrDeviceInfo& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderHelper::Create< ::rocvad::PrDeviceInfo, ::rocvad::PrDeviceInfo, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(channel_.get(), cq, rpcmethod_add_device_, context, request);
}

::grpc::ClientAsyncResponseReader< ::rocvad::PrDeviceInfo>* DriverProtocol::Stub::Asyncadd_deviceRaw(::grpc::ClientContext* context, const ::rocvad::PrDeviceInfo& request, ::grpc::CompletionQueue* cq) {
  auto* result =
    this->PrepareAsyncadd_deviceRaw(context, request, cq);
  result->StartCall();
  return result;
}

::grpc::Status DriverProtocol::Stub::delete_device(::grpc::ClientContext* context, const ::rocvad::PrDeviceSelector& request, ::rocvad::PrNone* response) {
  return ::grpc::internal::BlockingUnaryCall< ::rocvad::PrDeviceSelector, ::rocvad::PrNone, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(channel_.get(), rpcmethod_delete_device_, context, request, response);
}

void DriverProtocol::Stub::async::delete_device(::grpc::ClientContext* context, const ::rocvad::PrDeviceSelector* request, ::rocvad::PrNone* response, std::function<void(::grpc::Status)> f) {
  ::grpc::internal::CallbackUnaryCall< ::rocvad::PrDeviceSelector, ::rocvad::PrNone, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(stub_->channel_.get(), stub_->rpcmethod_delete_device_, context, request, response, std::move(f));
}

void DriverProtocol::Stub::async::delete_device(::grpc::ClientContext* context, const ::rocvad::PrDeviceSelector* request, ::rocvad::PrNone* response, ::grpc::ClientUnaryReactor* reactor) {
  ::grpc::internal::ClientCallbackUnaryFactory::Create< ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(stub_->channel_.get(), stub_->rpcmethod_delete_device_, context, request, response, reactor);
}

::grpc::ClientAsyncResponseReader< ::rocvad::PrNone>* DriverProtocol::Stub::PrepareAsyncdelete_deviceRaw(::grpc::ClientContext* context, const ::rocvad::PrDeviceSelector& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderHelper::Create< ::rocvad::PrNone, ::rocvad::PrDeviceSelector, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(channel_.get(), cq, rpcmethod_delete_device_, context, request);
}

::grpc::ClientAsyncResponseReader< ::rocvad::PrNone>* DriverProtocol::Stub::Asyncdelete_deviceRaw(::grpc::ClientContext* context, const ::rocvad::PrDeviceSelector& request, ::grpc::CompletionQueue* cq) {
  auto* result =
    this->PrepareAsyncdelete_deviceRaw(context, request, cq);
  result->StartCall();
  return result;
}

::grpc::Status DriverProtocol::Stub::bind(::grpc::ClientContext* context, const ::rocvad::PrEndpointRequest& request, ::rocvad::PrEndpointInfo* response) {
  return ::grpc::internal::BlockingUnaryCall< ::rocvad::PrEndpointRequest, ::rocvad::PrEndpointInfo, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(channel_.get(), rpcmethod_bind_, context, request, response);
}

void DriverProtocol::Stub::async::bind(::grpc::ClientContext* context, const ::rocvad::PrEndpointRequest* request, ::rocvad::PrEndpointInfo* response, std::function<void(::grpc::Status)> f) {
  ::grpc::internal::CallbackUnaryCall< ::rocvad::PrEndpointRequest, ::rocvad::PrEndpointInfo, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(stub_->channel_.get(), stub_->rpcmethod_bind_, context, request, response, std::move(f));
}

void DriverProtocol::Stub::async::bind(::grpc::ClientContext* context, const ::rocvad::PrEndpointRequest* request, ::rocvad::PrEndpointInfo* response, ::grpc::ClientUnaryReactor* reactor) {
  ::grpc::internal::ClientCallbackUnaryFactory::Create< ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(stub_->channel_.get(), stub_->rpcmethod_bind_, context, request, response, reactor);
}

::grpc::ClientAsyncResponseReader< ::rocvad::PrEndpointInfo>* DriverProtocol::Stub::PrepareAsyncbindRaw(::grpc::ClientContext* context, const ::rocvad::PrEndpointRequest& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderHelper::Create< ::rocvad::PrEndpointInfo, ::rocvad::PrEndpointRequest, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(channel_.get(), cq, rpcmethod_bind_, context, request);
}

::grpc::ClientAsyncResponseReader< ::rocvad::PrEndpointInfo>* DriverProtocol::Stub::AsyncbindRaw(::grpc::ClientContext* context, const ::rocvad::PrEndpointRequest& request, ::grpc::CompletionQueue* cq) {
  auto* result =
    this->PrepareAsyncbindRaw(context, request, cq);
  result->StartCall();
  return result;
}

::grpc::Status DriverProtocol::Stub::connect(::grpc::ClientContext* context, const ::rocvad::PrEndpointRequest& request, ::rocvad::PrEndpointInfo* response) {
  return ::grpc::internal::BlockingUnaryCall< ::rocvad::PrEndpointRequest, ::rocvad::PrEndpointInfo, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(channel_.get(), rpcmethod_connect_, context, request, response);
}

void DriverProtocol::Stub::async::connect(::grpc::ClientContext* context, const ::rocvad::PrEndpointRequest* request, ::rocvad::PrEndpointInfo* response, std::function<void(::grpc::Status)> f) {
  ::grpc::internal::CallbackUnaryCall< ::rocvad::PrEndpointRequest, ::rocvad::PrEndpointInfo, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(stub_->channel_.get(), stub_->rpcmethod_connect_, context, request, response, std::move(f));
}

void DriverProtocol::Stub::async::connect(::grpc::ClientContext* context, const ::rocvad::PrEndpointRequest* request, ::rocvad::PrEndpointInfo* response, ::grpc::ClientUnaryReactor* reactor) {
  ::grpc::internal::ClientCallbackUnaryFactory::Create< ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(stub_->channel_.get(), stub_->rpcmethod_connect_, context, request, response, reactor);
}

::grpc::ClientAsyncResponseReader< ::rocvad::PrEndpointInfo>* DriverProtocol::Stub::PrepareAsyncconnectRaw(::grpc::ClientContext* context, const ::rocvad::PrEndpointRequest& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderHelper::Create< ::rocvad::PrEndpointInfo, ::rocvad::PrEndpointRequest, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(channel_.get(), cq, rpcmethod_connect_, context, request);
}

::grpc::ClientAsyncResponseReader< ::rocvad::PrEndpointInfo>* DriverProtocol::Stub::AsyncconnectRaw(::grpc::ClientContext* context, const ::rocvad::PrEndpointRequest& request, ::grpc::CompletionQueue* cq) {
  auto* result =
    this->PrepareAsyncconnectRaw(context, request, cq);
  result->StartCall();
  return result;
}

DriverProtocol::Service::Service() {
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      DriverProtocol_method_names[0],
      ::grpc::internal::RpcMethod::NORMAL_RPC,
      new ::grpc::internal::RpcMethodHandler< DriverProtocol::Service, ::rocvad::PrNone, ::rocvad::PrNone, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(
          [](DriverProtocol::Service* service,
             ::grpc::ServerContext* ctx,
             const ::rocvad::PrNone* req,
             ::rocvad::PrNone* resp) {
               return service->ping(ctx, req, resp);
             }, this)));
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      DriverProtocol_method_names[1],
      ::grpc::internal::RpcMethod::NORMAL_RPC,
      new ::grpc::internal::RpcMethodHandler< DriverProtocol::Service, ::rocvad::PrNone, ::rocvad::PrDriverInfo, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(
          [](DriverProtocol::Service* service,
             ::grpc::ServerContext* ctx,
             const ::rocvad::PrNone* req,
             ::rocvad::PrDriverInfo* resp) {
               return service->driver_info(ctx, req, resp);
             }, this)));
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      DriverProtocol_method_names[2],
      ::grpc::internal::RpcMethod::SERVER_STREAMING,
      new ::grpc::internal::ServerStreamingHandler< DriverProtocol::Service, ::rocvad::PrNone, ::rocvad::PrLogEntry>(
          [](DriverProtocol::Service* service,
             ::grpc::ServerContext* ctx,
             const ::rocvad::PrNone* req,
             ::grpc::ServerWriter<::rocvad::PrLogEntry>* writer) {
               return service->stream_logs(ctx, req, writer);
             }, this)));
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      DriverProtocol_method_names[3],
      ::grpc::internal::RpcMethod::NORMAL_RPC,
      new ::grpc::internal::RpcMethodHandler< DriverProtocol::Service, ::rocvad::PrNone, ::rocvad::PrDeviceList, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(
          [](DriverProtocol::Service* service,
             ::grpc::ServerContext* ctx,
             const ::rocvad::PrNone* req,
             ::rocvad::PrDeviceList* resp) {
               return service->get_all_devices(ctx, req, resp);
             }, this)));
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      DriverProtocol_method_names[4],
      ::grpc::internal::RpcMethod::NORMAL_RPC,
      new ::grpc::internal::RpcMethodHandler< DriverProtocol::Service, ::rocvad::PrDeviceSelector, ::rocvad::PrDeviceInfo, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(
          [](DriverProtocol::Service* service,
             ::grpc::ServerContext* ctx,
             const ::rocvad::PrDeviceSelector* req,
             ::rocvad::PrDeviceInfo* resp) {
               return service->get_device(ctx, req, resp);
             }, this)));
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      DriverProtocol_method_names[5],
      ::grpc::internal::RpcMethod::NORMAL_RPC,
      new ::grpc::internal::RpcMethodHandler< DriverProtocol::Service, ::rocvad::PrDeviceInfo, ::rocvad::PrDeviceInfo, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(
          [](DriverProtocol::Service* service,
             ::grpc::ServerContext* ctx,
             const ::rocvad::PrDeviceInfo* req,
             ::rocvad::PrDeviceInfo* resp) {
               return service->add_device(ctx, req, resp);
             }, this)));
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      DriverProtocol_method_names[6],
      ::grpc::internal::RpcMethod::NORMAL_RPC,
      new ::grpc::internal::RpcMethodHandler< DriverProtocol::Service, ::rocvad::PrDeviceSelector, ::rocvad::PrNone, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(
          [](DriverProtocol::Service* service,
             ::grpc::ServerContext* ctx,
             const ::rocvad::PrDeviceSelector* req,
             ::rocvad::PrNone* resp) {
               return service->delete_device(ctx, req, resp);
             }, this)));
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      DriverProtocol_method_names[7],
      ::grpc::internal::RpcMethod::NORMAL_RPC,
      new ::grpc::internal::RpcMethodHandler< DriverProtocol::Service, ::rocvad::PrEndpointRequest, ::rocvad::PrEndpointInfo, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(
          [](DriverProtocol::Service* service,
             ::grpc::ServerContext* ctx,
             const ::rocvad::PrEndpointRequest* req,
             ::rocvad::PrEndpointInfo* resp) {
               return service->bind(ctx, req, resp);
             }, this)));
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      DriverProtocol_method_names[8],
      ::grpc::internal::RpcMethod::NORMAL_RPC,
      new ::grpc::internal::RpcMethodHandler< DriverProtocol::Service, ::rocvad::PrEndpointRequest, ::rocvad::PrEndpointInfo, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(
          [](DriverProtocol::Service* service,
             ::grpc::ServerContext* ctx,
             const ::rocvad::PrEndpointRequest* req,
             ::rocvad::PrEndpointInfo* resp) {
               return service->connect(ctx, req, resp);
             }, this)));
}

DriverProtocol::Service::~Service() {
}

::grpc::Status DriverProtocol::Service::ping(::grpc::ServerContext* context, const ::rocvad::PrNone* request, ::rocvad::PrNone* response) {
  (void) context;
  (void) request;
  (void) response;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}

::grpc::Status DriverProtocol::Service::driver_info(::grpc::ServerContext* context, const ::rocvad::PrNone* request, ::rocvad::PrDriverInfo* response) {
  (void) context;
  (void) request;
  (void) response;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}

::grpc::Status DriverProtocol::Service::stream_logs(::grpc::ServerContext* context, const ::rocvad::PrNone* request, ::grpc::ServerWriter< ::rocvad::PrLogEntry>* writer) {
  (void) context;
  (void) request;
  (void) writer;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}

::grpc::Status DriverProtocol::Service::get_all_devices(::grpc::ServerContext* context, const ::rocvad::PrNone* request, ::rocvad::PrDeviceList* response) {
  (void) context;
  (void) request;
  (void) response;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}

::grpc::Status DriverProtocol::Service::get_device(::grpc::ServerContext* context, const ::rocvad::PrDeviceSelector* request, ::rocvad::PrDeviceInfo* response) {
  (void) context;
  (void) request;
  (void) response;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}

::grpc::Status DriverProtocol::Service::add_device(::grpc::ServerContext* context, const ::rocvad::PrDeviceInfo* request, ::rocvad::PrDeviceInfo* response) {
  (void) context;
  (void) request;
  (void) response;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}

::grpc::Status DriverProtocol::Service::delete_device(::grpc::ServerContext* context, const ::rocvad::PrDeviceSelector* request, ::rocvad::PrNone* response) {
  (void) context;
  (void) request;
  (void) response;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}

::grpc::Status DriverProtocol::Service::bind(::grpc::ServerContext* context, const ::rocvad::PrEndpointRequest* request, ::rocvad::PrEndpointInfo* response) {
  (void) context;
  (void) request;
  (void) response;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}

::grpc::Status DriverProtocol::Service::connect(::grpc::ServerContext* context, const ::rocvad::PrEndpointRequest* request, ::rocvad::PrEndpointInfo* response) {
  (void) context;
  (void) request;
  (void) response;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}


}  // namespace rocvad

