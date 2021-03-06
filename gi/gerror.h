/* -*- mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; -*- */
/*
 * Copyright (c) 2008  litl, LLC
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

#ifndef __GJS_ERROR_H__
#define __GJS_ERROR_H__

#include <stdbool.h>
#include <glib.h>
#include <girepository.h>

#include "gi/wrapperutils.h"
#include "gjs/jsapi-util.h"
#include "gjs/macros.h"

class ErrorPrototype;
class ErrorInstance;

/* To conserve memory, we have two different kinds of private data for GError
 * JS wrappers: ErrorInstance, and ErrorPrototype. Both inherit from ErrorBase
 * for their common functionality. For more information, see the notes in
 * wrapperutils.h.
 *
 * ErrorPrototype, unlike the other GIWrapperPrototype subclasses, represents a
 * single error domain instead of a single GType. All Errors have a GType of
 * G_TYPE_ERROR.
 *
 * Note that in some situations GError structs can show up as BoxedInstance
 * instead of ErrorInstance. We have some special cases in this code to deal
 * with that.
 */

class ErrorBase
    : public GIWrapperBase<ErrorBase, ErrorPrototype, ErrorInstance> {
    friend class GIWrapperBase;

 protected:
    explicit ErrorBase(ErrorPrototype* proto = nullptr)
        : GIWrapperBase(proto) {}
    ~ErrorBase(void) {}

    static const GjsDebugTopic debug_topic = GJS_DEBUG_GERROR;
    static constexpr const char* debug_tag = "gerror";

    static const struct JSClassOps class_ops;
    static const struct JSClass klass;
    static JSPropertySpec proto_properties[];
    static JSFunctionSpec static_methods[];

    // Accessors

 public:
    GJS_USE GQuark domain(void) const;

    // Property getters

 protected:
    GJS_JSAPI_RETURN_CONVENTION
    static bool get_domain(JSContext* cx, unsigned argc, JS::Value* vp);
    GJS_JSAPI_RETURN_CONVENTION
    static bool get_message(JSContext* cx, unsigned argc, JS::Value* vp);
    GJS_JSAPI_RETURN_CONVENTION
    static bool get_code(JSContext* cx, unsigned argc, JS::Value* vp);

    // JS methods

    GJS_JSAPI_RETURN_CONVENTION
    static bool value_of(JSContext* cx, unsigned argc, JS::Value* vp);

 public:
    GJS_JSAPI_RETURN_CONVENTION
    static bool to_string(JSContext* cx, unsigned argc, JS::Value* vp);
};

class ErrorPrototype : public GIWrapperPrototype<ErrorBase, ErrorPrototype,
                                                 ErrorInstance, GIEnumInfo> {
    friend class GIWrapperPrototype;
    friend class GIWrapperBase;

    GQuark m_domain;

    explicit ErrorPrototype(GIEnumInfo* info, GType gtype);
    ~ErrorPrototype(void);

    GJS_JSAPI_RETURN_CONVENTION
    bool get_parent_proto(JSContext* cx, JS::MutableHandleObject proto) const;

 public:
    GJS_USE GQuark domain(void) const { return m_domain; }
};

class ErrorInstance : public GIWrapperInstance<ErrorBase, ErrorPrototype,
                                               ErrorInstance, GError> {
    friend class GIWrapperInstance;
    friend class GIWrapperBase;

    explicit ErrorInstance(JSContext* cx, JS::HandleObject obj);
    ~ErrorInstance(void);

 public:
    void copy_gerror(GError* other) { m_ptr = g_error_copy(other); }

    // Accessors

    GJS_USE const char* message(void) const { return m_ptr->message; }
    GJS_USE int code(void) const { return m_ptr->code; }

    // JS constructor

 private:
    GJS_JSAPI_RETURN_CONVENTION
    bool constructor_impl(JSContext* cx, JS::HandleObject obj,
                          const JS::CallArgs& args);
};

G_BEGIN_DECLS

GJS_JSAPI_RETURN_CONVENTION
bool gjs_define_error_class(JSContext* cx, JS::HandleObject in_object,
                            GIEnumInfo* info);
GJS_JSAPI_RETURN_CONVENTION
GError*   gjs_gerror_from_error        (JSContext             *context,
                                        JS::HandleObject       obj);
GJS_JSAPI_RETURN_CONVENTION
JSObject* gjs_error_from_gerror        (JSContext             *context,
                                        GError                *gerror,
                                        bool                   add_stack);
GJS_USE
bool      gjs_typecheck_gerror         (JSContext             *context,
                                        JS::HandleObject       obj,
                                        bool                   throw_error);

GJS_JSAPI_RETURN_CONVENTION
GError *gjs_gerror_make_from_error(JSContext       *cx,
                                   JS::HandleObject obj);

GJS_JSAPI_RETURN_CONVENTION
bool gjs_gerror_to_string(JSContext* cx, unsigned argc, JS::Value* vp);

GJS_JSAPI_RETURN_CONVENTION
bool gjs_define_error_properties(JSContext* cx, JS::HandleObject obj);

bool gjs_throw_gerror(JSContext* cx, GError* error);

G_END_DECLS

#endif  /* __GJS_ERROR_H__ */
