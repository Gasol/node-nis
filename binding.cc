#include <node.h>
#include <v8.h>
#include <rpcsvc/ypclnt.h>

using namespace v8;

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
    return scope.Close(obj);
}

void init(Handle<Object> exports, Handle<Object> module) {
    module->Set(
        String::NewSymbol("exports"),
        FunctionTemplate::New(CreateObject)->GetFunction()
    );
}

NODE_MODULE(nis, init);
