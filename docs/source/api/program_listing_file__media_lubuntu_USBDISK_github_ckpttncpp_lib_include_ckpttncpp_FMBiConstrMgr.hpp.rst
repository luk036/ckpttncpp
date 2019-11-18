
.. _program_listing_file__media_lubuntu_USBDISK_github_ckpttncpp_lib_include_ckpttncpp_FMBiConstrMgr.hpp:

Program Listing for File FMBiConstrMgr.hpp
==========================================

|exhale_lsh| :ref:`Return to documentation for file <file__media_lubuntu_USBDISK_github_ckpttncpp_lib_include_ckpttncpp_FMBiConstrMgr.hpp>` (``/media/lubuntu/USBDISK/github/ckpttncpp/lib/include/ckpttncpp/FMBiConstrMgr.hpp``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   
   #include "FMConstrMgr.hpp" // import FMConstrMgr
   
   struct FMBiConstrMgr : FMConstrMgr
   {
       FMBiConstrMgr(SimpleNetlist& H, double BalTol)
           : FMConstrMgr {H, BalTol, 2}
       {
       }
   
       FMBiConstrMgr(SimpleNetlist& H, double BalTol, uint8_t /*K*/)
           : FMConstrMgr {H, BalTol, 2}
       {
       }
   
       auto select_togo() const -> uint8_t
       {
           return this->diff[0] < this->diff[1] ? 0 : 1;
       }
   };
