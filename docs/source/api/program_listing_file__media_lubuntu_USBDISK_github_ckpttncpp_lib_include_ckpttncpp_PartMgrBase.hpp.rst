
.. _program_listing_file__media_lubuntu_USBDISK_github_ckpttncpp_lib_include_ckpttncpp_PartMgrBase.hpp:

Program Listing for File PartMgrBase.hpp
========================================

|exhale_lsh| :ref:`Return to documentation for file <file__media_lubuntu_USBDISK_github_ckpttncpp_lib_include_ckpttncpp_PartMgrBase.hpp>` (``/media/lubuntu/USBDISK/github/ckpttncpp/lib/include/ckpttncpp/PartMgrBase.hpp``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   
   // **Special code for two-pin nets**
   // Take a snapshot when a move make **negative** gain.
   // Snapshot in the form of "interface"???
   #include "netlist.hpp"
   #include <cinttypes>
   #include <iterator>
   #include <py2cpp/py2cpp.hpp>
   #include <type_traits>
   #include <vector>
   
   template <typename GainMgr, typename ConstrMgr,
       template <typename _GainMgr, typename _ConstrMgr> class Derived> //
   class PartMgrBase
   {
   
     public:
       using GainCalc_ = typename GainMgr::GainCalc_;
       using GainMgr_ = GainMgr;
       using ConstrMgr_ = ConstrMgr;
   
       using Der = Derived<GainMgr, ConstrMgr>;
       Der& self = *static_cast<Der*>(this);
   
     protected:
       SimpleNetlist& H;
       GainMgr& gainMgr;
       ConstrMgr& validator;
       uint8_t K;
       // std::vector<uint8_t> snapshot;
       // std::vector<uint8_t> part;
   
     public:
       int totalcost {0};
   
       PartMgrBase(SimpleNetlist& H, GainMgr& gainMgr, ConstrMgr& constrMgr)
           : H {H}
           , gainMgr {gainMgr}
           , validator {constrMgr}
           , K {gainMgr.K}
       {
       }
   
       void init(gsl::span<uint8_t> part);
   
       size_t legalize(gsl::span<uint8_t> part);
   
       void optimize(gsl::span<uint8_t> part);
   
     private:
       void __optimize_1pass(gsl::span<uint8_t> part);
   };
