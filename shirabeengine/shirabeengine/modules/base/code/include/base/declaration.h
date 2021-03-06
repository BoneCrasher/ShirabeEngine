/*!
 * @file      declaration.h
 * @author    Marc-Anton Boehm-von Thenen
 * @date      27/07/2018
 * @copyright SmartRay GmbH (www.smartray.com)
 */

#ifndef __SHIRABE_BASE_DECLARATION_H__
#define __SHIRABE_BASE_DECLARATION_H__

#include <platform/platform.h>

// Class definition helpers for more explicit interfaces
// IMPORTANT:
//   Discuss as exception to the rule, to have the code look natural.
#define public_typedefs             public
#define public_interfaces           public
#define public_classes              public
#define public_structs              public
#define public_enums                public
#define public_static_constants     public
#define public_static_fields        public
#define public_static_functions     public
#define public_constructors         public
#define public_destructors          public
#define public_operators            public
#define public_methods              public
#define public_constants            public
#define public_members              public
#define public_api                  public
#define protected_typedefs          protected
#define protected_interfaces        protected
#define protected_classes           protected
#define protected_structs           protected
#define protected_enums             protected
#define protected_static_constants  protected
#define protected_static_fields     protected
#define protected_static_functions  protected
#define protected_constructors      protected
#define protected_destructors       protected
#define protected_operators         protected
#define protected_methods           protected
#define protected_constants         protected
#define protected_members           protected
#define protected_api               protected
#define private_typedefs            private
#define private_interfaces          private
#define private_classes             private
#define private_structs             private
#define private_enums               private
#define private_static_constants    private
#define private_static_fields       private
#define private_static_functions    private
#define private_constructors        private
#define private_destructors         private
#define private_operators           private
#define private_methods             private
#define private_constants           private
#define private_members             private
#define private_api                 private

#define SHIRABE_INLINE    inline
#define SHIRABE_UNUSED(x) (void)(x)
#define SHIRABE_EXPLICIT_DISCARD(x) { [[maybe_unused]] auto __impl_v = x; }

#define SHIRABE_DEBUG_BREAK __builtin_trap()

#define SHIRABE_ASSERT(condition)                   \
            if(!(condition))                        \
            {                                       \
                SHIRABE_DEBUG_BREAK;                \
            }

#define SHIRABE_ASSERT_TEXT(condition, format, ...) \
            if(!(condition))                        \
            {                                       \
                SHIRABE_DEBUG_BREAK;                \
            }

#define SHIRABE_RETURN_IF(condition, value) \
            if(condition)                   \
            {                               \
                return value;               \
            }

#endif
