#include <node.h>
#include <nan.h>
#include <rpcsvc/ypclnt.h>

using namespace v8;

extern "C" {
    int foreach_all(unsigned long instatus, char *inkey, int inkeylen, char *inval, int invallen, void *data);
}

NAN_METHOD(First) {
    HandleScope scope;
    int error;

    if (args.Length() < 1) {
        ThrowException(Exception::TypeError(String::New("Not enough arguments")));
        return scope.Close(Undefined());
    }
    if (!args[0]->IsString()) {
        ThrowException(Exception::TypeError(String::New("First argument must be string")));
        return scope.Close(Undefined());
    }

    String::Utf8Value mapname(args[0]->ToString());
    String::Utf8Value domain(args.Holder()->Get(NanSymbol("domain_name")));
    char *outkey, *outval;
    int outkeylen, outvallen;

    error = yp_first(*domain, *mapname, &outkey, &outkeylen, &outval, &outvallen);
    if (error) {
        Local<String> errorMessage = String::New(yperr_string(error));
        ThrowException(Exception::Error(errorMessage));
    }

    Local<Object> obj = Object::New();
    obj->Set(String::New("key"), String::New(outkey, outkeylen));
    obj->Set(String::New("value"), String::New(outval, outvallen));
    return obj;
}

NAN_METHOD(Master) {
    HandleScope scope;
    int error;

    if (args.Length() < 1) {
        ThrowException(Exception::TypeError(String::New("Not enough arguments")));
        return scope.Close(Undefined());
    }
    if (!args[0]->IsString()) {
        ThrowException(Exception::TypeError(String::New("First argument must be string")));
        return scope.Close(Undefined());
    }

    String::Utf8Value mapname(args[0]->ToString());
    String::Utf8Value domain(args.Holder()->Get(NanSymbol("domain_name")));
    char *outname;

    error = yp_master(*domain, *mapname, &outname);
    if (error) {
        Local<String> errorMessage = String::New(yperr_string(error));
        ThrowException(Exception::Error(errorMessage));
    }

    return String::New(outname);
}

NAN_METHOD(Order) {
    HandleScope scope;
    int error;

    if (args.Length() < 1) {
        ThrowException(Exception::TypeError(String::New("Not enough arguments")));
        return scope.Close(Undefined());
    }
    if (!args[0]->IsString()) {
        ThrowException(Exception::TypeError(String::New("First argument must be string")));
        return scope.Close(Undefined());
    }

    String::Utf8Value mapname(args[0]->ToString());
    String::Utf8Value domain(args.Holder()->Get(NanSymbol("domain_name")));
    int outorder;

    error = yp_order(*domain, *mapname, &outorder);
    if (error) {
        Local<String> errorMessage = String::New(yperr_string(error));
        ThrowException(Exception::Error(errorMessage));
    }

    return Integer::New(outorder);
}

NAN_METHOD(All) {
    HandleScope scope;
    int error;

    if (args.Length() < 2) {
        ThrowException(Exception::TypeError(String::New("Not enough arguments")));
        return scope.Close(Undefined());
    }
    if (!args[0]->IsString()) {
        ThrowException(Exception::TypeError(String::New("First argument must be string")));
        return scope.Close(Undefined());
    }
    if (!args[1]->IsFunction()) {
        ThrowException(Exception::TypeError(String::New("Second argument must be function")));
        return scope.Close(Undefined());
    }
    Local<Function> user_cb = Local<Function>::Cast(args[1]);
    struct ypall_callback cb;
    cb.foreach = foreach_all;
    cb.data = (char *) &user_cb;

    String::Utf8Value mapname(args[0]->ToString());
    String::Utf8Value domain(args.Holder()->Get(NanSymbol("domain_name")));

    error = yp_all(*domain, *mapname, &cb);
    if (error) {
        Local<String> errorMessage = String::New(yperr_string(error));
        ThrowException(Exception::Error(errorMessage));
    }

    return scope.Close(Undefined());
}

extern "C"
int foreach_all(unsigned long instatus, char *inkey, int inkeylen, char *inval, int invallen, void *data) {
    Local<Function> *cb = static_cast<Local<Function> *>(data);
    const int argc = 3;
    Local<Value> argv[argc] = {
        Local<Value>::New(Integer::NewFromUnsigned(instatus)),
        Local<Value>::New(String::New(inkey, inkeylen)),
        Local<Value>::New(String::New(inval, invallen))
    };
    Local<Value> ret = (*cb)->Call(Context::GetCurrent()->Global(), argc, argv);
    return ret->Int32Value();
}

NAN_METHOD(CreateObject) {
    HandleScope scope;
    char* domp;
    int error;

    Local<Object> obj = Object::New();
    error = yp_get_default_domain(&domp);
    if (error) {
        Local<String> errorMessage = String::New(yperr_string(error));
        ThrowException(Exception::Error(errorMessage));
    }
    obj->Set(NanSymbol("domain_name"), String::New(domp));
    obj->Set(NanSymbol("all"), FunctionTemplate::New(All)->GetFunction());
    obj->Set(NanSymbol("order"), FunctionTemplate::New(Order)->GetFunction());
    obj->Set(NanSymbol("master"), FunctionTemplate::New(Master)->GetFunction());
    obj->Set(NanSymbol("first"), FunctionTemplate::New(First)->GetFunction());
    return scope.Close(obj);
}

void Initialize(Handle<Object> exports, Handle<Object> module) {
    module->Set(
        NanSymbol("exports"),
        FunctionTemplate::New(CreateObject)->GetFunction()
    );
}

NODE_MODULE(nis, Initialize);
