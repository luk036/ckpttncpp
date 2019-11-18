
.. _program_listing_file__media_lubuntu_USBDISK_github_ckpttncpp_lib_include_ckpttncpp_MLPartMgr.hpp:

Program Listing for File MLPartMgr.hpp
======================================

|exhale_lsh| :ref:`Return to documentation for file <file__media_lubuntu_USBDISK_github_ckpttncpp_lib_include_ckpttncpp_MLPartMgr.hpp>` (``/media/lubuntu/USBDISK/github/ckpttncpp/lib/include/ckpttncpp/MLPartMgr.hpp``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   
   // **Special code for two-pin nets**
   // Take a snapshot when a move make **negative** gain.
   // Snapshot in the form of "interface"???
   #include "FMPartMgr.hpp" // import FMPartMgr
   #include "netlist.hpp"
   #include <cassert>
   #include <iostream>
   #include <vector>
   
   extern std::unique_ptr<SimpleNetlist> create_contraction_subgraph(
       SimpleNetlist&, const py::set<node_t>&);
   
   class MLPartMgr
   {
     private:
       double BalTol;
       uint8_t K;
   
     public:
       int totalcost {0};
   
       explicit MLPartMgr(double BalTol, uint8_t K = 2)
           : BalTol {BalTol}
           , K {K}
       {
       }
   
       template <typename PartMgr>
       auto run_FMPartition(SimpleNetlist& H, gsl::span<uint8_t> part,
           size_t limitsize = 7) -> size_t
       {
           using GainMgr = typename PartMgr::GainMgr_;
           using ConstrMgr = typename PartMgr::ConstrMgr_;
   
           // auto gainMgr = GainMgr{H, this->K};
           // auto constrMgr = ConstrMgr{H, this->BalTol, this->K};
           // auto partMgr = PartMgr{H, gainMgr, constrMgr};
           auto gainMgrPtr = std::make_unique<GainMgr>(H, this->K);
           auto constrMgrPtr =
               std::make_unique<ConstrMgr>(H, this->BalTol, this->K);
           auto partMgrPtr =
               std::make_unique<PartMgr>(H, *gainMgrPtr, *constrMgrPtr);
   
           // partMgrPtr->init(part);
           auto legalcheck = partMgrPtr->legalize(part);
           if (legalcheck != 2)
           {
               this->totalcost = partMgrPtr->totalcost;
               return legalcheck;
           }
           if (H.number_of_modules() >= limitsize)
           { // OK
               try
               {
                   auto H2 = create_contraction_subgraph(H, py::set<node_t> {});
                   if (5 * H2->number_of_modules() <= 3 * H.number_of_modules())
                   {
                       auto part2 =
                           std::vector<uint8_t>(H2->number_of_modules(), 0);
                       // auto extern_nets_ss = py::set<node_t>{};
                       // auto part2_info =
                       //     PartInfo{std::move(part2),
                       //     std::move(extern_nets_ss)};
                       H2->projection_up(part, part2);
                       legalcheck =
                           this->run_FMPartition<PartMgr>(*H2, part2, limitsize);
                       if (legalcheck == 2)
                       {
                           H2->projection_down(part2, part);
                       }
                   }
               }
               catch (std::bad_alloc)
               {
                   std::cerr << "Warning: Insufficient memory."
                             << " Discard one level." << '\n';
               }
           }
           partMgrPtr->optimize(part);
           assert(partMgrPtr->totalcost >= 0);
           this->totalcost = partMgrPtr->totalcost;
   
           return legalcheck;
       }
   
       template <typename PartMgr>
       auto run_Partition(SimpleNetlist& H, gsl::span<uint8_t> part,
           size_t limitsize = 7) -> size_t
       {
           using GainMgr = typename PartMgr::GainMgr_;
           using ConstrMgr = typename PartMgr::ConstrMgr_;
   
           auto gainMgr = GainMgr {H, this->K};
           auto constrMgr = ConstrMgr {H, this->BalTol, this->K};
           auto partMgr = PartMgr {H, gainMgr, constrMgr};
           // partMgr.init(part);
           auto legalcheck = partMgr.legalize(part);
           if (legalcheck != 2)
           {
               this->totalcost = partMgr.totalcost;
               return legalcheck;
           }
           this->run_Partition_recur<PartMgr>(H, part, limitsize);
           return legalcheck;
       }
   
       template <typename PartMgr>
       auto run_Partition_recur(
           SimpleNetlist& H, gsl::span<uint8_t> part, size_t limitsize) -> void
       {
           if (H.number_of_modules() >= limitsize)
           { // OK
               try
               {
                   auto H2 = create_contraction_subgraph(H, py::set<node_t> {});
                   if (5 * H2->number_of_modules() <= 3 * H.number_of_modules())
                   {
                       auto part2 =
                           std::vector<uint8_t>(H2->number_of_modules(), 0);
                       // auto extern_nets_ss = py::set<node_t>{};
                       // auto part2_info =
                       //     PartInfo{std::move(part2),
                       //     std::move(extern_nets_ss)};
                       H2->projection_up(part, part2);
                       this->run_Partition_recur<PartMgr>(*H2, part2, limitsize);
                       H2->projection_down(part2, part);
                   }
               }
               catch (std::bad_alloc)
               {
                   std::cerr << "Warning: Insufficient memory."
                             << " Discard one level." << '\n';
               }
           }
           using GainMgr = typename PartMgr::GainMgr_;
           using ConstrMgr = typename PartMgr::ConstrMgr_;
           // auto gainMgr = GainMgr{H, this->K};
           // auto constrMgr = ConstrMgr{H, this->BalTol, this->K};
           // auto partMgr = PartMgr{H, gainMgr, constrMgr};
           auto gainMgrPtr = std::make_unique<GainMgr>(H, this->K);
           auto constrMgrPtr =
               std::make_unique<ConstrMgr>(H, this->BalTol, this->K);
           auto partMgrPtr =
               std::make_unique<PartMgr>(H, *gainMgrPtr, *constrMgrPtr);
           partMgrPtr->optimize(part);
           assert(partMgrPtr->totalcost >= 0);
           this->totalcost = partMgrPtr->totalcost;
       }
   };
