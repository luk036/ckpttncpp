
.. _program_listing_file__media_lubuntu_USBDISK_github_ckpttncpp_lib_include_ckpttncpp_FMConstrMgr.hpp:

Program Listing for File FMConstrMgr.hpp
========================================

|exhale_lsh| :ref:`Return to documentation for file <file__media_lubuntu_USBDISK_github_ckpttncpp_lib_include_ckpttncpp_FMConstrMgr.hpp>` (``/media/lubuntu/USBDISK/github/ckpttncpp/lib/include/ckpttncpp/FMConstrMgr.hpp``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   
   #include "netlist.hpp"
   #include <algorithm>
   #include <cinttypes>
   #include <cmath>
   #include <gsl/span>
   #include <vector>
   
   // Check if (the move of v can satisfied, makebetter, or notsatisfied
   
   class FMConstrMgr
   {
       int weight; // cache value
   
     protected:
       SimpleNetlist& H;
       double BalTol;
       uint8_t K;
       std::vector<int> diff;
       int lowerbound {0};
       int totalweight {0};
   
       FMConstrMgr(SimpleNetlist& H, double BalTol, uint8_t K = 2)
           : H {H}
           , BalTol {BalTol}
           , K {K}
           , diff(K, 0)
       {
           for (auto v : this->H.modules)
           {
               weight = this->H.get_module_weight(v);
               this->totalweight += weight;
           }
           auto totalweightK = this->totalweight * (2. / this->K);
           this->lowerbound = std::round(totalweightK * this->BalTol);
       }
   
     public:
       auto init(gsl::span<const uint8_t> part) -> void;
   
       auto check_legal(const MoveInfoV& move_info_v) -> size_t;
   
       auto check_constraints(const MoveInfoV& move_info_v) -> bool;
   
       auto update_move(const MoveInfoV& move_info_v) -> void;
   };
