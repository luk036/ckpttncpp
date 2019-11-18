
.. _program_listing_file__media_lubuntu_USBDISK_github_ckpttncpp_lib_include_xnetwork_exception.hpp:

Program Listing for File exception.hpp
======================================

|exhale_lsh| :ref:`Return to documentation for file <file__media_lubuntu_USBDISK_github_ckpttncpp_lib_include_xnetwork_exception.hpp>` (``/media/lubuntu/USBDISK/github/ckpttncpp/lib/include/xnetwork/exception.hpp``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   // -*- coding: utf-8 -*-
   #ifndef _HOME_UBUNTU_GITHUB_XNETWORK_EXCEPTION_HPP
   #define _HOME_UBUNTU_GITHUB_XNETWORK_EXCEPTION_HPP 1
   
   #include <exception>
   #include <initializer_list>
   #include <stdexcept>
   
   namespace xn
   {
   
   // static const auto __all__ = {
   //     "HasACycle",
   //     "NodeNotFound",
   //     "PowerIterationFailedConvergence",
   //     "ExceededMaxIterations",
   //     "AmbiguousSolution",
   //     "XNetworkAlgorithmError",
   //     "XNetworkException",
   //     "XNetworkError",
   //     "XNetworkNoCycle",
   //     "XNetworkNoPath",
   //     "XNetworkNotImplemented",
   //     "XNetworkPointlessConcept",
   //     "XNetworkUnbounded",
   //     "XNetworkUnfeasible",
   // };
   
   struct XNetworkException : std::runtime_error
   {
       explicit XNetworkException(const char* msg)
           : std::runtime_error(msg)
       {
       }
   };
   
   struct XNetworkError : XNetworkException
   {
       explicit XNetworkError(const char* msg)
           : XNetworkException(msg)
       {
       }
   };
   
   struct XNetworkPointlessConcept : XNetworkException
   {
       explicit XNetworkPointlessConcept(const char* msg)
           : XNetworkException(msg)
       {
       }
   };
   
   struct XNetworkAlgorithmError : XNetworkException
   {
       explicit XNetworkAlgorithmError(const char* msg)
           : XNetworkException(msg)
       {
       }
   };
   
   struct XNetworkUnfeasible : XNetworkAlgorithmError
   {
       explicit XNetworkUnfeasible(const char* msg)
           : XNetworkAlgorithmError(msg)
       {
       }
   };
   
   struct XNetworkNoPath : XNetworkUnfeasible
   {
       explicit XNetworkNoPath(const char* msg)
           : XNetworkUnfeasible(msg)
       {
       }
   };
   
   struct XNetworkNoCycle : XNetworkUnfeasible
   {
       explicit XNetworkNoCycle(const char* msg)
           : XNetworkUnfeasible(msg)
       {
       }
   };
   
   struct HasACycle : XNetworkException
   {
       explicit HasACycle(const char* msg)
           : XNetworkException(msg)
       {
       }
   };
   
   struct XNetworkUnbounded : XNetworkAlgorithmError
   {
       explicit XNetworkUnbounded(const char* msg)
           : XNetworkAlgorithmError(msg)
       {
       }
   };
   
   struct XNetworkNotImplemented : XNetworkException
   {
       explicit XNetworkNotImplemented(const char* msg)
           : XNetworkException(msg)
       {
       }
   };
   
   struct NodeNotFound : XNetworkException
   {
       explicit NodeNotFound(const char* msg)
           : XNetworkException(msg)
       {
       }
   };
   
   struct AmbiguousSolution : XNetworkException
   {
       explicit AmbiguousSolution(const char* msg)
           : XNetworkException(msg)
       {
       }
   };
   
   struct ExceededMaxIterations : XNetworkException
   {
       explicit ExceededMaxIterations(const char* msg)
           : XNetworkException(msg)
       {
       }
   };
   
   //     /** Raised when the power iteration method fails to converge within a
   //     specified iteration limit.
   //
   //     `num_iterations` is the number of iterations that have been
   //     completed when this exception was raised.
   //
   //      */
   // class PowerIterationFailedConvergence : ExceededMaxIterations {
   //     explicit _Self( num_iterations, *args, **kw) {
   //         const auto msg = "power iteration failed to converge within {}
   //         iterations"; exception_message = msg.format(num_iterations);
   //         superinit = super(PowerIterationFailedConvergence, *this).__init__
   //         superinit( exception_message, *args, **kw);
   //     }
   // };
   }; // namespace xn
   
   #endif
