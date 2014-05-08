#include <node.h>
#include <v8.h>
#include <rpcsvc/ypclnt.h>

using namespace v8;

extern "C" {
    int foreach_all(unsigned long instatus, char *inkey, int inkeylen, char *inval, int invallen, void *data);
}

Handle<Value> All(const Arguments& args) {
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
    String::Utf8Value domain(args.Holder()->Get(String::NewSymbol("domain_name")));

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

Handle<Value> CreateObject(const Arguments& args) {
    HandleScope scope;
    char* domp;
    int error;

    Local<Object> obj = Object::New();
    error = yp_get_default_domain(&domp);
    if (error) {
        Local<String> errorMessage = String::New(yperr_string(error));
        ThrowException(Exception::Error(errorMessage));
    }
    obj->Set(String::NewSymbol("domain_name"), String::New(domp));
    obj->Set(
        String::NewSymbol("all"),
        FunctionTemplate::New(All)->GetFunction()
    );
    return scope.Close(obj);
}

void init(Handle<Object> exports, Handle<Object> module) {
    module->Set(
        String::NewSymbol("exports"),
        FunctionTemplate::New(CreateObject)->GetFunction()
    );
}

NODE_MODULE(nis, init);
