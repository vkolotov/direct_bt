/*
 * Author: Sven Gothel <sgothel@jausoft.com>
 * Copyright (c) 2020 Gothel Software e.K.
 * Copyright (c) 2020 ZAFENA AB
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
 * LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 * OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include "jau_direct_bt_DBTGattService.h"

// #define VERBOSE_ON 1
#include <jau/debug.hpp>

#include "helper_base.hpp"
#include "helper_dbt.hpp"

#include "direct_bt/BTDevice.hpp"
#include "direct_bt/BTAdapter.hpp"
#include "direct_bt/BTGattService.hpp"

using namespace direct_bt;
using namespace jau::jni;

void Java_jau_direct_1bt_DBTGattService_deleteImpl(JNIEnv *env, jobject obj, jlong nativeInstance) {
    (void)obj;
    try {
        shared_ptr_ref<BTGattService> sref(nativeInstance, false /* throw_on_nullptr */); // hold copy until done
        if( nullptr != sref.pointer() ) {
            std::shared_ptr<BTGattService>* sref_ptr = castInstance<BTGattService>(nativeInstance);
            delete sref_ptr;
        }
    } catch(...) {
        rethrow_and_raise_java_exception(env);
    }
}

jstring Java_jau_direct_1bt_DBTGattService_toStringImpl(JNIEnv *env, jobject obj) {
    try {
        shared_ptr_ref<BTGattService> service(env, obj); // hold until done
        JavaAnonRef service_java = service->getJavaObject(); // hold until done!
        JavaGlobalObj::check(service_java, E_FILE_LINE);
        return from_string_to_jstring(env, service->toString());
    } catch(...) {
        rethrow_and_raise_java_exception(env);
    }
    return nullptr;
}


static const std::string _characteristicClazzCtorArgs("(JLjau/direct_bt/DBTGattService;SLorg/direct_bt/GattCharPropertySet;Ljava/lang/String;SII)V");
static const std::string _gattCharPropSetClassName("org/direct_bt/GattCharPropertySet");
static const std::string _gattCharPropSetClazzCtorArgs("(B)V");

jobject Java_jau_direct_1bt_DBTGattService_getCharsImpl(JNIEnv *env, jobject obj) {
    try {
        shared_ptr_ref<BTGattService> service(env, obj); // hold until done
        JavaAnonRef service_java = service->getJavaObject(); // hold until done!
        if( !JavaGlobalObj::isValid(service_java) ) {
            ERR_PRINT("DBTGattService.getCharsImpl: invalid Java service object: %s", service->toString().c_str());
        }
        JavaGlobalObj::check(service_java, E_FILE_LINE);

        jau::darray<std::shared_ptr<BTGattChar>> & characteristics = service->characteristicList;
        WARN_PRINT("DBTGattService.getCharsImpl: service has %zu chars: %s",
                characteristics.size(), service->toString().c_str());

        jclass gattCharPropSetClazz;
        jmethodID gattCharPropSetClazzCtor;
        // gattCharPropSetClazzRef, gattCharPropSetClazzCtor
        {
            gattCharPropSetClazz = search_class(env, _gattCharPropSetClassName.c_str());
            java_exception_check_and_throw(env, E_FILE_LINE);
            if( nullptr == gattCharPropSetClazz ) {
                throw jau::InternalError("BTDevice::java_class not found: "+_gattCharPropSetClassName, E_FILE_LINE);
            }
        }
        gattCharPropSetClazzCtor = search_method(env, gattCharPropSetClazz, "<init>", _gattCharPropSetClazzCtorArgs.c_str(), false);
        java_exception_check_and_throw(env, E_FILE_LINE);
        if( nullptr == gattCharPropSetClazzCtor ) {
            throw jau::InternalError("GattCharPropertySet ctor not found: "+_gattCharPropSetClassName+".<init>"+_gattCharPropSetClazzCtorArgs, E_FILE_LINE);
        }

        /**
            DBTGattChar(final long nativeInstance, final DBTGattService service,
                        final short handle, final GattCharPropertySet properties,
                        final String value_type_uuid, final short value_handle,
                        final int clientCharacteristicsConfigIndex,
                        final int userDescriptionIndex)
        */
        jau::function<jobject(JNIEnv*, jclass, jmethodID, const BTGattCharRef&)> ctor_char =
                [&gattCharPropSetClazz, &gattCharPropSetClazzCtor](JNIEnv *env_, jclass clazz, jmethodID clazz_ctor, const BTGattCharRef& characteristic)->jobject {
                    WARN_PRINT("DBTGattService.getCharsImpl: ctor begin char native %p: %s",
                            characteristic.get(), nullptr != characteristic ? characteristic->toString().c_str() : "null");
                    // prepare adapter ctor
                    std::shared_ptr<BTGattService> _service = characteristic->getServiceUnchecked();
                    if( nullptr == _service ) {
                        throw jau::RuntimeException("Characteristic's service null: "+characteristic->toString(), E_FILE_LINE);
                    }
                    WARN_PRINT("DBTGattService.getCharsImpl: ctor char native %p belongs to service native %p: %s",
                            characteristic.get(), _service.get(), _service->toString().c_str());
                    JavaAnonRef _service_java = _service->getJavaObject(); // hold until done!
                    if( !JavaGlobalObj::isValid(_service_java) ) {
                        ERR_PRINT("DBTGattService.getCharsImpl: invalid Java service object while creating char %s in %s",
                                characteristic->toString().c_str(), _service->toString().c_str());
                    }
                    WARN_PRINT("DBTGattService.getCharsImpl: checking Java service object for char native %p: %s",
                            characteristic.get(), characteristic->toString().c_str());
                    JavaGlobalObj::check(_service_java, E_FILE_LINE);

                    jobject jservice = JavaGlobalObj::GetObject(_service_java);
                    WARN_PRINT("DBTGattService.getCharsImpl: Java service object %p for char native %p",
                            jservice, characteristic.get());

                    jobject jGattCharPropSet = env_->NewObject(gattCharPropSetClazz, gattCharPropSetClazzCtor, (jbyte)characteristic->properties);
                    java_exception_check_and_throw(env_, E_FILE_LINE);
                    WARN_PRINT("DBTGattService.getCharsImpl: Java property object %p for char native %p props 0x%02x",
                            jGattCharPropSet, characteristic.get(), (unsigned int)characteristic->properties);
                    JNIGlobalRef::check(jGattCharPropSet, E_FILE_LINE);
                    java_exception_check_and_throw(env_, E_FILE_LINE);

                    const jstring uuid = from_string_to_jstring(env_, characteristic->value_type->toUUID128String());
                    java_exception_check_and_throw(env_, E_FILE_LINE);
                    WARN_PRINT("DBTGattService.getCharsImpl: Java UUID object %p for char native %p uuid %s handle 0x%04x value_handle 0x%04x cccd %d user_desc %d",
                            uuid, characteristic.get(), characteristic->value_type->toUUID128String().c_str(),
                            characteristic->handle, characteristic->value_handle,
                            characteristic->clientCharConfigIndex, characteristic->userDescriptionIndex);

                    shared_ptr_ref<BTGattChar> characteristic_sref(characteristic); // new instance to be released into new jobject
                    const jlong native_handle = characteristic_sref.release_to_jlong();
                    WARN_PRINT("DBTGattService.getCharsImpl: NewObject begin char native %p exported handle 0x%llx: %s",
                            characteristic.get(), (long long)native_handle, characteristic->toString().c_str());
                    jobject jcharVal = env_->NewObject(clazz, clazz_ctor, native_handle, jservice,
                            characteristic->handle, jGattCharPropSet,
                            uuid, characteristic->value_handle,
                            characteristic->clientCharConfigIndex,
                            characteristic->userDescriptionIndex);
                    java_exception_check_and_throw(env_, E_FILE_LINE);
                    WARN_PRINT("DBTGattService.getCharsImpl: NewObject returned %p for char native %p exported handle 0x%llx",
                            jcharVal, characteristic.get(), (long long)native_handle);
                    if( nullptr == jcharVal ) {
                        ERR_PRINT("DBTGattService.getCharsImpl: NewObject returned null for char %s in %s",
                                characteristic->toString().c_str(), _service->toString().c_str());
                    }
                    WARN_PRINT("DBTGattService.getCharsImpl: checking Java char local ref %p for char native %p",
                            jcharVal, characteristic.get());
                    JNIGlobalRef::check(jcharVal, E_FILE_LINE);
                    JavaAnonRef jCharRef = characteristic->getJavaObject(); // GlobalRef
                    WARN_PRINT("DBTGattService.getCharsImpl: native char %p getJavaObject valid %d: %s",
                            characteristic.get(), JavaGlobalObj::isValid(jCharRef), characteristic->toString().c_str());
                    if( !JavaGlobalObj::isValid(jCharRef) ) {
                        ERR_PRINT("DBTGattService.getCharsImpl: invalid Java char object after ctor: %s in %s",
                                characteristic->toString().c_str(), _service->toString().c_str());
                    }
                    WARN_PRINT("DBTGattService.getCharsImpl: checking Java char global ref for native %p",
                            characteristic.get());
                    JavaGlobalObj::check(jCharRef, E_FILE_LINE);
                    env_->DeleteLocalRef(jGattCharPropSet);
                    env_->DeleteLocalRef(jcharVal);
                    jobject res = JavaGlobalObj::GetObject(jCharRef);
                    WARN_PRINT("DBTGattService.getCharsImpl: ctor end char native %p -> Java object %p",
                            characteristic.get(), res);
                    return res;
                };
        jobject jres = convert_vector_sharedptr_to_jarraylist<jau::darray<std::shared_ptr<BTGattChar>>, BTGattChar>(
                env, characteristics, _characteristicClazzCtorArgs.c_str(), ctor_char);
        env->DeleteLocalRef(gattCharPropSetClazz);
        return jres;
    } catch(...) {
        rethrow_and_raise_java_exception(env);
    }
    return nullptr;
}

