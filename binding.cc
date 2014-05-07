#include <node.h>
#include <v8.h>

using namespace v8;

Handle<Value> CreateObject(const Arguments& args) {
    HandleScope scope;

    Local<Object> obj = Object::New();
    return scope.Close(obj);
}

void init(Handle<Object> exports, Handle<Object> module) {
    module->Set(String::NewSymbol("exports"),
            FunctionTemplate::New(CreateObject)->GetFunction());
}

NODE_MODULE(nis, init);
