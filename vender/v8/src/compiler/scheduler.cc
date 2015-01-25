// Copyright 2013 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <deque>
#include <queue>

#include "src/compiler/scheduler.h"

#include "src/bit-vector.h"
#include "src/compiler/control-equivalence.h"
#include "src/compiler/graph.h"
#include "src/compiler/graph-inl.h"
#include "src/compiler/node.h"
#include "src/compiler/node-marker.h"
#include "src/compiler/node-properties-inl.h"

namespace v8 {
namespace internal {
namespace compiler {

static inline void Trace(const char* msg, ...) {
  if (FLAG_trace_turbo_scheduler) {
    va_list arguments;
    va_start(arguments, msg);
    base::OS::VPrint(msg, arguments);
    va_end(arguments);
  }
}


Scheduler::Scheduler(Zone* zone, Graph* graph, Schedule* schedule)
    : zone_(zone),
      graph_(graph),
      schedule_(schedule),
      scheduled_nodes_(zone),
      schedule_root_nodes_(zone),
      schedule_queue_(zone),
      node_data_(graph_->NodeCount(), DefaultSchedulerData(), zone) {}


Schedule* Scheduler::ComputeSchedule(Zone* zone, Graph* graph) {
  Schedule* schedule = new (graph->zone())
      Schedule(graph->zone(), static_cast<size_t>(graph->NodeCount()));
  Scheduler scheduler(zone, graph, schedule);

  scheduler.BuildCFG();
  scheduler.ComputeSpecialRPONumbering();
  scheduler.GenerateImmediateDominatorTree();

  scheduler.PrepareUses();
  scheduler.ScheduleEarly();
  scheduler.ScheduleLate();

  scheduler.SealFinalSchedule();

  return schedule;
}


Scheduler::SchedulerData Scheduler::DefaultSchedulerData() {
  SchedulerData def = {schedule_->start(), 0, kUnknown};
  return def;
}


Scheduler::SchedulerData* Scheduler::GetData(Node* node) {
  DCHECK(node->id() < static_cast<int>(node_data_.size()));
  return &node_data_[node->id()];
}


Scheduler::Placement Scheduler::GetPlacement(Node* node) {
  SchedulerData* data = GetData(node);
  if (data->placement_ == kUnknown) {  // Compute placement, once, on demand.
    switch (node->opcode()) {
      case IrOpcode::kParameter:
      case IrOpcode::kOsrValue:
        // Parameters and OSR values are always fixed to the start block.
        data->placement_ = kFixed;
        break;
      case IrOpcode::kPhi:
      case IrOpcode::kEffectPhi: {
        // Phis and effect phis are fixed if their control inputs are, whereas
        // otherwise they are coupled to a floating control node.
        Placement p = GetPlacement(NodeProperties::GetControlInput(node));
        data->placement_ = (p == kFixed ? kFixed : kCoupled);
        break;
      }
#define DEFINE_CONTROL_CASE(V) case IrOpcode::k##V:
      CONTROL_OP_LIST(DEFINE_CONTROL_CASE)
#undef DEFINE_CONTROL_CASE
      {
        // Control nodes that were not control-reachable from end may float.
        data->placement_ = kSchedulable;
        break;
      }
      default:
        data->placement_ = kSchedulable;
        break;
    }
  }
  return data->placement_;
}


void Scheduler::UpdatePlacement(Node* node, Placement placement) {
  SchedulerData* data = GetData(node);
  if (data->placement_ != kUnknown) {  // Trap on mutation, not initialization.
    switch (node->opcode()) {
      case IrOpcode::kParameter:
        // Parameters are fixed once and for all.
        UNREACHABLE();
        break;
      case IrOpcode::kPhi:
      case IrOpcode::kEffectPhi: {
        // Phis and effect phis are coupled to their respective blocks.
        DCHECK_EQ(Scheduler::kCoupled, data->placement_);
        DCHECK_EQ(Scheduler::kFixed, placement);
        Node* control = NodeProperties::GetControlInput(node);
        BasicBlock* block = schedule_->block(control);
        schedule_->AddNode(block, node);
        break;
      }
#define DEFINE_CONTROL_CASE(V) case IrOpcode::k##V:
      CONTROL_OP_LIST(DEFINE_CONTROL_CASE)
#undef DEFINE_CONTROL_CASE
      {
        // Control nodes force coupled uses to be placed.
        for (auto use : node->uses()) {
          if (GetPlacement(use) == Scheduler::kCoupled) {
            DCHECK_EQ(node, NodeProperties::GetControlInput(use));
            UpdatePlacement(use, placement);
          }
        }
        break;
      }
      default:
        DCHECK_EQ(Scheduler::kSchedulable, data->placement_);
        DCHECK_EQ(Scheduler::kScheduled, placement);
        break;
    }
    // Reduce the use count of the node's inputs to potentially make them
    // schedulable. If all the uses of a node have been scheduled, then the node
    // itself can be scheduled.
    for (Edge const edge : node->input_edges()) {
      DecrementUnscheduledUseCount(edge.to(), edge.index(), edge.from());
    }
  }
  data->placement_ = placement;
}


bool Scheduler::IsCoupledControlEdge(Node* node, int index) {
  return GetPlacement(node) == kCoupled &&
         NodeProperties::FirstControlIndex(node) == index;
}


void Scheduler::IncrementUnscheduledUseCount(Node* node, int index,
                                             Node* from) {
  // Make sure that control edges from coupled nodes are not counted.
  if (IsCoupledControlEdge(from, index)) return;

  // Tracking use counts for fixed nodes is useless.
  if (GetPlacement(node) == kFixed) return;

  // Use count for coupled nodes is summed up on their control.
  if (GetPlacement(node) == kCoupled) {
    Node* control = NodeProperties::GetControlInput(node);
    return IncrementUnscheduledUseCount(control, index, from);
  }

  ++(GetData(node)->unscheduled_count_);
  if (FLAG_trace_turbo_scheduler) {
    Trace("  Use count of #%d:%s (used by #%d:%s)++ = %d\n", node->id(),
          node->op()->mnemonic(), from->id(), from->op()->mnemonic(),
          GetData(node)->unscheduled_count_);
  }
}


void Scheduler::DecrementUnscheduledUseCount(Node* node, int index,
                                             Node* from) {
  // Make sure that control edges from coupled nodes are not counted.
  if (IsCoupledControlEdge(from, index)) return;

  // Tracking use counts for fixed nodes is useless.
  if (GetPlacement(node) == kFixed) return;

  // Use count for coupled nodes is summed up on their control.
  if (GetPlacement(node) == kCoupled) {
    Node* control = NodeProperties::GetControlInput(node);
    return DecrementUnscheduledUseCount(control, index, from);
  }

  DCHECK(GetData(node)->unscheduled_count_ > 0);
  --(GetData(node)->unscheduled_count_);
  if (FLAG_trace_turbo_scheduler) {
    Trace("  Use count of #%d:%s (used by #%d:%s)-- = %d\n", node->id(),
          node->op()->mnemonic(), from->id(), from->op()->mnemonic(),
          GetData(node)->unscheduled_count_);
  }
  if (GetData(node)->unscheduled_count_ == 0) {
    Trace("    newly eligible #%d:%s\n", node->id(), node->op()->mnemonic());
    schedule_queue_.push(node);
  }
}


BasicBlock* Scheduler::GetCommonDominator(BasicBlock* b1, BasicBlock* b2) {
  while (b1 != b2) {
    int32_t b1_depth = b1->dominator_depth();
    int32_t b2_depth = b2->dominator_depth();
    if (b1_depth < b2_depth) {
      b2 = b2->dominator();
    } else {
      b1 = b1->dominator();
    }
  }
  return b1;
}


// -----------------------------------------------------------------------------
// Phase 1: Build control-flow graph.


// Internal class to build a control flow graph (i.e the basic blocks and edges
// between them within a Schedule) from the node graph. Visits control edges of
// the graph backwards from an end node in order to find the connected control
// subgraph, needed for scheduling.
class CFGBuilder : public ZoneObject {
 public:
  CFGBuilder(Zone* zone, Scheduler* scheduler)
      : scheduler_(scheduler),
        schedule_(scheduler->schedule_),
        queued_(scheduler->graph_, 2),
        queue_(zone),
        control_(zone),
        component_entry_(NULL),
        component_start_(NULL),
        component_end_(NULL) {}

  // Run the control flow graph construction algorithm by walking the graph
  // backwards from end through control edges, building and connecting the
  // basic blocks for control nodes.
  void Run() {
    ResetDataStructures();
    Queue(scheduler_->graph_->end());

    while (!queue_.empty()) {  // Breadth-first backwards traversal.
      Node* node = queue_.front();
      queue_.pop();
      int max = NodeProperties::PastControlIndex(node);
      for (int i = NodeProperties::FirstControlIndex(node); i < max; i++) {
        Queue(node->InputAt(i));
      }
    }

    for (NodeVector::iterator i = control_.begin(); i != control_.end(); ++i) {
      ConnectBlocks(*i);  // Connect block to its predecessor/successors.
    }
  }

  // Run the control flow graph construction for a minimal control-connected
  // component ending in {exit} and merge that component into an existing
  // control flow graph at the bottom of {block}.
  void Run(BasicBlock* block, Node* exit) {
    ResetDataStructures();
    Queue(exit);

    component_entry_ = NULL;
    component_start_ = block;
    component_end_ = schedule_->block(exit);
    scheduler_->equivalence_->Run(exit);
    while (!queue_.empty()) {  // Breadth-first backwards traversal.
      Node* node = queue_.front();
      queue_.pop();

      // Use control dependence equivalence to find a canonical single-entry
      // single-exit region that makes up a minimal component to be scheduled.
      if (IsSingleEntrySingleExitRegion(node, exit)) {
        Trace("Found SESE at #%d:%s\n", node->id(), node->op()->mnemonic());
        DCHECK_EQ(NULL, component_entry_);
        component_entry_ = node;
        continue;
      }

      int max = NodeProperties::PastControlIndex(node);
      for (int i = NodeProperties::FirstControlIndex(node); i < max; i++) {
        Queue(node->InputAt(i));
      }
    }
    DCHECK_NE(NULL, component_entry_);

    for (NodeVector::iterator i = control_.begin(); i != control_.end(); ++i) {
      ConnectBlocks(*i);  // Connect block to its predecessor/successors.
    }
  }

 private:
  // TODO(mstarzinger): Only for Scheduler::FuseFloatingControl.
  friend class Scheduler;

  void FixNode(BasicBlock* block, Node* node) {
    schedule_->AddNode(block, node);
    scheduler_->UpdatePlacement(node, Scheduler::kFixed);
  }

  void Queue(Node* node) {
    // Mark the connected control nodes as they are queued.
    if (!queued_.Get(node)) {
      BuildBlocks(node);
      queue_.push(node);
      queued_.Set(node, true);
      control_.push_back(node);
    }
  }

  void BuildBlocks(Node* node) {
    switch (node->opcode()) {
      case IrOpcode::kEnd:
        FixNode(schedule_->end(), node);
        break;
      case IrOpcode::kStart:
        FixNode(schedule_->start(), node);
        break;
      case IrOpcode::kLoop:
      case IrOpcode::kMerge:
        BuildBlockForNode(node);
        break;
      case IrOpcode::kTerminate: {
        // Put Terminate in the loop to which it refers.
        Node* loop = NodeProperties::GetControlInput(node);
        BasicBlock* block = BuildBlockForNode(loop);
        FixNode(block, node);
        break;
      }
      case IrOpcode::kBranch:
        BuildBlocksForSuccessors(node, IrOpcode::kIfTrue, IrOpcode::kIfFalse);
        break;
      default:
        break;
    }
  }

  void ConnectBlocks(Node* node) {
    switch (node->opcode()) {
      case IrOpcode::kLoop:
      case IrOpcode::kMerge:
        ConnectMerge(node);
        break;
      case IrOpcode::kBranch:
        scheduler_->UpdatePlacement(node, Scheduler::kFixed);
        ConnectBranch(node);
        break;
      case IrOpcode::kReturn:
        scheduler_->UpdatePlacement(node, Scheduler::kFixed);
        ConnectReturn(node);
        break;
      default:
        break;
    }
  }

  BasicBlock* BuildBlockForNode(Node* node) {
    BasicBlock* block = schedule_->block(node);
    if (block == NULL) {
      block = schedule_->NewBasicBlock();
      Trace("Create block B%d for #%d:%s\n", block->id().ToInt(), node->id(),
            node->op()->mnemonic());
      FixNode(block, node);
    }
    return block;
  }

  void BuildBlocksForSuccessors(Node* node, IrOpcode::Value a,
                                IrOpcode::Value b) {
    Node* successors[2];
    CollectSuccessorProjections(node, successors, a, b);
    BuildBlockForNode(successors[0]);
    BuildBlockForNode(successors[1]);
  }

  // Collect the branch-related projections from a node, such as IfTrue,
  // IfFalse.
  // TODO(titzer): consider moving this to node.h
  void CollectSuccessorProjections(Node* node, Node** buffer,
                                   IrOpcode::Value true_opcode,
                                   IrOpcode::Value false_opcode) {
    buffer[0] = NULL;
    buffer[1] = NULL;
    for (Node* use : node->uses()) {
      if (use->opcode() == true_opcode) {
        DCHECK_EQ(NULL, buffer[0]);
        buffer[0] = use;
      }
      if (use->opcode() == false_opcode) {
        DCHECK_EQ(NULL, buffer[1]);
        buffer[1] = use;
      }
    }
    DCHECK_NE(NULL, buffer[0]);
    DCHECK_NE(NULL, buffer[1]);
  }

  void CollectSuccessorBlocks(Node* node, BasicBlock** buffer,
                              IrOpcode::Value true_opcode,
                              IrOpcode::Value false_opcode) {
    Node* successors[2];
    CollectSuccessorProjections(node, successors, true_opcode, false_opcode);
    buffer[0] = schedule_->block(successors[0]);
    buffer[1] = schedule_->block(successors[1]);
  }

  void ConnectBranch(Node* branch) {
    BasicBlock* successor_blocks[2];
    CollectSuccessorBlocks(branch, successor_blocks, IrOpcode::kIfTrue,
                           IrOpcode::kIfFalse);

    // Consider branch hints.
    switch (BranchHintOf(branch->op())) {
      case BranchHint::kNone:
        break;
      case BranchHint::kTrue:
        successor_blocks[1]->set_deferred(true);
        break;
      case BranchHint::kFalse:
        successor_blocks[0]->set_deferred(true);
        break;
    }

    if (branch == component_entry_) {
      TraceConnect(branch, component_start_, successor_blocks[0]);
      TraceConnect(branch, component_start_, successor_blocks[1]);
      schedule_->InsertBranch(component_start_, component_end_, branch,
                              successor_blocks[0], successor_blocks[1]);
    } else {
      Node* branch_block_node = NodeProperties::GetControlInput(branch);
      BasicBlock* branch_block = schedule_->block(branch_block_node);
      DCHECK(branch_block != NULL);

      TraceConnect(branch, branch_block, successor_blocks[0]);
      TraceConnect(branch, branch_block, successor_blocks[1]);
      schedule_->AddBranch(branch_block, branch, successor_blocks[0],
                           successor_blocks[1]);
    }
  }

  void ConnectMerge(Node* merge) {
    // Don't connect the special merge at the end to its predecessors.
    if (IsFinalMerge(merge)) return;

    BasicBlock* block = schedule_->block(merge);
    DCHECK(block != NULL);
    // For all of the merge's control inputs, add a goto at the end to the
    // merge's basic block.
    for (Node* const input : merge->inputs()) {
      BasicBlock* predecessor_block = schedule_->block(input);
      TraceConnect(merge, predecessor_block, block);
      schedule_->AddGoto(predecessor_block, block);
    }
  }

  void ConnectReturn(Node* ret) {
    Node* return_block_node = NodeProperties::GetControlInput(ret);
    BasicBlock* return_block = schedule_->block(return_block_node);
    TraceConnect(ret, return_block, NULL);
    schedule_->AddReturn(return_block, ret);
  }

  void TraceConnect(Node* node, BasicBlock* block, BasicBlock* succ) {
    DCHECK_NE(NULL, block);
    if (succ == NULL) {
      Trace("Connect #%d:%s, B%d -> end\n", node->id(), node->op()->mnemonic(),
            block->id().ToInt());
    } else {
      Trace("Connect #%d:%s, B%d -> B%d\n", node->id(), node->op()->mnemonic(),
            block->id().ToInt(), succ->id().ToInt());
    }
  }

  bool IsFinalMerge(Node* node) {
    return (node->opcode() == IrOpcode::kMerge &&
            node == scheduler_->graph_->end()->InputAt(0));
  }

  bool IsSingleEntrySingleExitRegion(Node* entry, Node* exit) const {
    size_t entry_class = scheduler_->equivalence_->ClassOf(entry);
    size_t exit_class = scheduler_->equivalence_->ClassOf(exit);
    return entry != exit && entry_class == exit_class;
  }

  void ResetDataStructures() {
    control_.clear();
    DCHECK(queue_.empty());
    DCHECK(control_.empty());
  }

  Scheduler* scheduler_;
  Schedule* schedule_;
  NodeMarker<bool> queued_;      // Mark indicating whether node is queued.
  ZoneQueue<Node*> queue_;       // Queue used for breadth-first traversal.
  NodeVector control_;           // List of encountered control nodes.
  Node* component_entry_;        // Component single-entry node.
  BasicBlock* component_start_;  // Component single-entry block.
  BasicBlock* component_end_;    // Component single-exit block.
};


void Scheduler::BuildCFG() {
  Trace("--- CREATING CFG -------------------------------------------\n");

  // Instantiate a new control equivalence algorithm for the graph.
  equivalence_ = new (zone_) ControlEquivalence(zone_, graph_);

  // Build a control-flow graph for the main control-connected component that
  // is being spanned by the graph's start and end nodes.
  control_flow_builder_ = new (zone_) CFGBuilder(zone_, this);
  control_flow_builder_->Run();

  // Initialize per-block data.
  scheduled_nodes_.resize(schedule_->BasicBlockCount(), NodeVector(zone_));
}


// -----------------------------------------------------------------------------
// Phase 2: Compute special RPO and dominator tree.


// Compute the special reverse-post-order block ordering, which is essentially
// a RPO of the graph where loop bodies are contiguous. Properties:
// 1. If block A is a predecessor of B, then A appears before B in the order,
//    unless B is a loop header and A is in the loop headed at B
//    (i.e. A -> B is a backedge).
// => If block A dominates block B, then A appears before B in the order.
// => If block A is a loop header, A appears before all blocks in the loop
//    headed at A.
// 2. All loops are contiguous in the order (i.e. no intervening blocks that
//    do not belong to the loop.)
// Note a simple RPO traversal satisfies (1) but not (2).
class SpecialRPONumberer : public ZoneObject {
 public:
  SpecialRPONumberer(Zone* zone, Schedule* schedule)
      : zone_(zone),
        schedule_(schedule),
        order_(NULL),
        beyond_end_(NULL),
        loops_(zone),
        backedges_(zone),
        stack_(zone),
        previous_block_count_(0) {}

  // Computes the special reverse-post-order for the main control flow graph,
  // that is for the graph spanned between the schedule's start and end blocks.
  void ComputeSpecialRPO() {
    DCHECK(schedule_->end()->SuccessorCount() == 0);
    DCHECK_EQ(NULL, order_);  // Main order does not exist yet.
    ComputeAndInsertSpecialRPO(schedule_->start(), schedule_->end());
  }

  // Computes the special reverse-post-order for a partial control flow graph,
  // that is for the graph spanned between the given {entry} and {end} blocks,
  // then updates the existing ordering with this new information.
  void UpdateSpecialRPO(BasicBlock* entry, BasicBlock* end) {
    DCHECK_NE(NULL, order_);  // Main order to be updated is present.
    ComputeAndInsertSpecialRPO(entry, end);
  }

  // Serialize the previously computed order as a special reverse-post-order
  // numbering for basic blocks into the final schedule.
  void SerializeRPOIntoSchedule() {
    int32_t number = 0;
    for (BasicBlock* b = order_; b != NULL; b = b->rpo_next()) {
      b->set_rpo_number(number++);
      schedule_->rpo_order()->push_back(b);
    }
    BeyondEndSentinel()->set_rpo_number(number);
  }

  // Print and verify the special reverse-post-order.
  void PrintAndVerifySpecialRPO() {
#if DEBUG
    if (FLAG_trace_turbo_scheduler) PrintRPO();
    VerifySpecialRPO();
#endif
  }

 private:
  typedef std::pair<BasicBlock*, size_t> Backedge;

  // Numbering for BasicBlock::rpo_number for this block traversal:
  static const int kBlockOnStack = -2;
  static const int kBlockVisited1 = -3;
  static const int kBlockVisited2 = -4;
  static const int kBlockUnvisited1 = -1;
  static const int kBlockUnvisited2 = kBlockVisited1;

  struct SpecialRPOStackFrame {
    BasicBlock* block;
    size_t index;
  };

  struct LoopInfo {
    BasicBlock* header;
    ZoneList<BasicBlock*>* outgoing;
    BitVector* members;
    LoopInfo* prev;
    BasicBlock* end;
    BasicBlock* start;

    void AddOutgoing(Zone* zone, BasicBlock* block) {
      if (outgoing == NULL) {
        outgoing = new (zone) ZoneList<BasicBlock*>(2, zone);
      }
      outgoing->Add(block, zone);
    }
  };

  int Push(ZoneVector<SpecialRPOStackFrame>& stack, int depth,
           BasicBlock* child, int unvisited) {
    if (child->rpo_number() == unvisited) {
      stack[depth].block = child;
      stack[depth].index = 0;
      child->set_rpo_number(kBlockOnStack);
      return depth + 1;
    }
    return depth;
  }

  BasicBlock* PushFront(BasicBlock* head, BasicBlock* block) {
    block->set_rpo_next(head);
    return block;
  }

  static int GetLoopNumber(BasicBlock* block) { return block->loop_number(); }
  static void SetLoopNumber(BasicBlock* block, int loop_number) {
    return block->set_loop_number(loop_number);
  }
  static bool HasLoopNumber(BasicBlock* block) {
    return block->loop_number() >= 0;
  }

  // TODO(mstarzinger): We only need this special sentinel because some tests
  // use the schedule's end block in actual control flow (e.g. with end having
  // successors). Once this has been cleaned up we can use the end block here.
  BasicBlock* BeyondEndSentinel() {
    if (beyond_end_ == NULL) {
      BasicBlock::Id id = BasicBlock::Id::FromInt(-1);
      beyond_end_ = new (schedule_->zone()) BasicBlock(schedule_->zone(), id);
    }
    return beyond_end_;
  }

  // Compute special RPO for the control flow graph between {entry} and {end},
  // mutating any existing order so that the result is still valid.
  void ComputeAndInsertSpecialRPO(BasicBlock* entry, BasicBlock* end) {
    // RPO should not have been serialized for this schedule yet.
    CHECK_EQ(kBlockUnvisited1, schedule_->start()->loop_number());
    CHECK_EQ(kBlockUnvisited1, schedule_->start()->rpo_number());
    CHECK_EQ(0, static_cast<int>(schedule_->rpo_order()->size()));

    // Find correct insertion point within existing order.
    BasicBlock* insertion_point = entry->rpo_next();
    BasicBlock* order = insertion_point;

    // Perform an iterative RPO traversal using an explicit stack,
    // recording backedges that form cycles. O(|B|).
    DCHECK_LT(previous_block_count_, schedule_->BasicBlockCount());
    stack_.resize(schedule_->BasicBlockCount() - previous_block_count_);
    previous_block_count_ = schedule_->BasicBlockCount();
    int stack_depth = Push(stack_, 0, entry, kBlockUnvisited1);
    int num_loops = static_cast<int>(loops_.size());

    while (stack_depth > 0) {
      int current = stack_depth - 1;
      SpecialRPOStackFrame* frame = &stack_[current];

      if (frame->block != end &&
          frame->index < frame->block->SuccessorCount()) {
        // Process the next successor.
        BasicBlock* succ = frame->block->SuccessorAt(frame->index++);
        if (succ->rpo_number() == kBlockVisited1) continue;
        if (succ->rpo_number() == kBlockOnStack) {
          // The successor is on the stack, so this is a backedge (cycle).
          backedges_.push_back(Backedge(frame->block, frame->index - 1));
          if (!HasLoopNumber(succ)) {
            // Assign a new loop number to the header if it doesn't have one.
            SetLoopNumber(succ, num_loops++);
          }
        } else {
          // Push the successor onto the stack.
          DCHECK(succ->rpo_number() == kBlockUnvisited1);
          stack_depth = Push(stack_, stack_depth, succ, kBlockUnvisited1);
        }
      } else {
        // Finished with all successors; pop the stack and add the block.
        order = PushFront(order, frame->block);
        frame->block->set_rpo_number(kBlockVisited1);
        stack_depth--;
      }
    }

    // If no loops were encountered, then the order we computed was correct.
    if (num_loops > static_cast<int>(loops_.size())) {
      // Otherwise, compute the loop information from the backedges in order
      // to perform a traversal that groups loop bodies together.
      ComputeLoopInfo(stack_, num_loops, &backedges_);

      // Initialize the "loop stack". Note the entry could be a loop header.
      LoopInfo* loop =
          HasLoopNumber(entry) ? &loops_[GetLoopNumber(entry)] : NULL;
      order = insertion_point;

      // Perform an iterative post-order traversal, visiting loop bodies before
      // edges that lead out of loops. Visits each block once, but linking loop
      // sections together is linear in the loop size, so overall is
      // O(|B| + max(loop_depth) * max(|loop|))
      stack_depth = Push(stack_, 0, entry, kBlockUnvisited2);
      while (stack_depth > 0) {
        SpecialRPOStackFrame* frame = &stack_[stack_depth - 1];
        BasicBlock* block = frame->block;
        BasicBlock* succ = NULL;

        if (block != end && frame->index < block->SuccessorCount()) {
          // Process the next normal successor.
          succ = block->SuccessorAt(frame->index++);
        } else if (HasLoopNumber(block)) {
          // Process additional outgoing edges from the loop header.
          if (block->rpo_number() == kBlockOnStack) {
            // Finish the loop body the first time the header is left on the
            // stack.
            DCHECK(loop != NULL && loop->header == block);
            loop->start = PushFront(order, block);
            order = loop->end;
            block->set_rpo_number(kBlockVisited2);
            // Pop the loop stack and continue visiting outgoing edges within
            // the context of the outer loop, if any.
            loop = loop->prev;
            // We leave the loop header on the stack; the rest of this iteration
            // and later iterations will go through its outgoing edges list.
          }

          // Use the next outgoing edge if there are any.
          int outgoing_index =
              static_cast<int>(frame->index - block->SuccessorCount());
          LoopInfo* info = &loops_[GetLoopNumber(block)];
          DCHECK(loop != info);
          if (block != entry && info->outgoing != NULL &&
              outgoing_index < info->outgoing->length()) {
            succ = info->outgoing->at(outgoing_index);
            frame->index++;
          }
        }

        if (succ != NULL) {
          // Process the next successor.
          if (succ->rpo_number() == kBlockOnStack) continue;
          if (succ->rpo_number() == kBlockVisited2) continue;
          DCHECK(succ->rpo_number() == kBlockUnvisited2);
          if (loop != NULL && !loop->members->Contains(succ->id().ToInt())) {
            // The successor is not in the current loop or any nested loop.
            // Add it to the outgoing edges of this loop and visit it later.
            loop->AddOutgoing(zone_, succ);
          } else {
            // Push the successor onto the stack.
            stack_depth = Push(stack_, stack_depth, succ, kBlockUnvisited2);
            if (HasLoopNumber(succ)) {
              // Push the inner loop onto the loop stack.
              DCHECK(GetLoopNumber(succ) < num_loops);
              LoopInfo* next = &loops_[GetLoopNumber(succ)];
              next->end = order;
              next->prev = loop;
              loop = next;
            }
          }
        } else {
          // Finished with all successors of the current block.
          if (HasLoopNumber(block)) {
            // If we are going to pop a loop header, then add its entire body.
            LoopInfo* info = &loops_[GetLoopNumber(block)];
            for (BasicBlock* b = info->start; true; b = b->rpo_next()) {
              if (b->rpo_next() == info->end) {
                b->set_rpo_next(order);
                info->end = order;
                break;
              }
            }
            order = info->start;
          } else {
            // Pop a single node off the stack and add it to the order.
            order = PushFront(order, block);
            block->set_rpo_number(kBlockVisited2);
          }
          stack_depth--;
        }
      }
    }

    // Publish new order the first time.
    if (order_ == NULL) order_ = order;

    // Compute the correct loop headers and set the correct loop ends.
    LoopInfo* current_loop = NULL;
    BasicBlock* current_header = entry->loop_header();
    int32_t loop_depth = entry->loop_depth();
    if (entry->IsLoopHeader()) --loop_depth;  // Entry might be a loop header.
    for (BasicBlock* b = order; b != insertion_point; b = b->rpo_next()) {
      BasicBlock* current = b;

      // Reset BasicBlock::rpo_number again.
      current->set_rpo_number(kBlockUnvisited1);

      // Finish the previous loop(s) if we just exited them.
      while (current_header != NULL && current == current_header->loop_end()) {
        DCHECK(current_header->IsLoopHeader());
        DCHECK(current_loop != NULL);
        current_loop = current_loop->prev;
        current_header = current_loop == NULL ? NULL : current_loop->header;
        --loop_depth;
      }
      current->set_loop_header(current_header);

      // Push a new loop onto the stack if this loop is a loop header.
      if (HasLoopNumber(current)) {
        ++loop_depth;
        current_loop = &loops_[GetLoopNumber(current)];
        BasicBlock* end = current_loop->end;
        current->set_loop_end(end == NULL ? BeyondEndSentinel() : end);
        current_header = current_loop->header;
        Trace("B%d is a loop header, increment loop depth to %d\n",
              current->id().ToInt(), loop_depth);
      }

      current->set_loop_depth(loop_depth);

      if (current->loop_header() == NULL) {
        Trace("B%d is not in a loop (depth == %d)\n", current->id().ToInt(),
              current->loop_depth());
      } else {
        Trace("B%d has loop header B%d, (depth == %d)\n", current->id().ToInt(),
              current->loop_header()->id().ToInt(), current->loop_depth());
      }
    }
  }

  // Computes loop membership from the backedges of the control flow graph.
  void ComputeLoopInfo(ZoneVector<SpecialRPOStackFrame>& queue,
                       size_t num_loops, ZoneVector<Backedge>* backedges) {
    // Extend existing loop membership vectors.
    for (LoopInfo& loop : loops_) {
      BitVector* new_members = new (zone_)
          BitVector(static_cast<int>(schedule_->BasicBlockCount()), zone_);
      new_members->CopyFrom(*loop.members);
      loop.members = new_members;
    }

    // Extend loop information vector.
    loops_.resize(num_loops, LoopInfo());

    // Compute loop membership starting from backedges.
    // O(max(loop_depth) * max(|loop|)
    for (size_t i = 0; i < backedges->size(); i++) {
      BasicBlock* member = backedges->at(i).first;
      BasicBlock* header = member->SuccessorAt(backedges->at(i).second);
      size_t loop_num = GetLoopNumber(header);
      if (loops_[loop_num].header == NULL) {
        loops_[loop_num].header = header;
        loops_[loop_num].members = new (zone_)
            BitVector(static_cast<int>(schedule_->BasicBlockCount()), zone_);
      }

      int queue_length = 0;
      if (member != header) {
        // As long as the header doesn't have a backedge to itself,
        // Push the member onto the queue and process its predecessors.
        if (!loops_[loop_num].members->Contains(member->id().ToInt())) {
          loops_[loop_num].members->Add(member->id().ToInt());
        }
        queue[queue_length++].block = member;
      }

      // Propagate loop membership backwards. All predecessors of M up to the
      // loop header H are members of the loop too. O(|blocks between M and H|).
      while (queue_length > 0) {
        BasicBlock* block = queue[--queue_length].block;
        for (size_t i = 0; i < block->PredecessorCount(); i++) {
          BasicBlock* pred = block->PredecessorAt(i);
          if (pred != header) {
            if (!loops_[loop_num].members->Contains(pred->id().ToInt())) {
              loops_[loop_num].members->Add(pred->id().ToInt());
              queue[queue_length++].block = pred;
            }
          }
        }
      }
    }
  }

#if DEBUG
  void PrintRPO() {
    OFStream os(stdout);
    os << "RPO with " << loops_.size() << " loops";
    if (loops_.size() > 0) {
      os << " (";
      for (size_t i = 0; i < loops_.size(); i++) {
        if (i > 0) os << " ";
        os << "B" << loops_[i].header->id();
      }
      os << ")";
    }
    os << ":\n";

    for (BasicBlock* block = order_; block != NULL; block = block->rpo_next()) {
      BasicBlock::Id bid = block->id();
      // TODO(jarin,svenpanne): Add formatting here once we have support for
      // that in streams (we want an equivalent of PrintF("%5d:", x) here).
      os << "  " << block->rpo_number() << ":";
      for (size_t i = 0; i < loops_.size(); i++) {
        bool range = loops_[i].header->LoopContains(block);
        bool membership = loops_[i].header != block && range;
        os << (membership ? " |" : "  ");
        os << (range ? "x" : " ");
      }
      os << "  B" << bid << ": ";
      if (block->loop_end() != NULL) {
        os << " range: [" << block->rpo_number() << ", "
           << block->loop_end()->rpo_number() << ")";
      }
      if (block->loop_header() != NULL) {
        os << " header: B" << block->loop_header()->id();
      }
      if (block->loop_depth() > 0) {
        os << " depth: " << block->loop_depth();
      }
      os << "\n";
    }
  }

  void VerifySpecialRPO() {
    BasicBlockVector* order = schedule_->rpo_order();
    DCHECK(order->size() > 0);
    DCHECK((*order)[0]->id().ToInt() == 0);  // entry should be first.

    for (size_t i = 0; i < loops_.size(); i++) {
      LoopInfo* loop = &loops_[i];
      BasicBlock* header = loop->header;
      BasicBlock* end = header->loop_end();

      DCHECK(header != NULL);
      DCHECK(header->rpo_number() >= 0);
      DCHECK(header->rpo_number() < static_cast<int>(order->size()));
      DCHECK(end != NULL);
      DCHECK(end->rpo_number() <= static_cast<int>(order->size()));
      DCHECK(end->rpo_number() > header->rpo_number());
      DCHECK(header->loop_header() != header);

      // Verify the start ... end list relationship.
      int links = 0;
      BasicBlock* block = loop->start;
      DCHECK_EQ(header, block);
      bool end_found;
      while (true) {
        if (block == NULL || block == loop->end) {
          end_found = (loop->end == block);
          break;
        }
        // The list should be in same order as the final result.
        DCHECK(block->rpo_number() == links + header->rpo_number());
        links++;
        block = block->rpo_next();
        DCHECK(links < static_cast<int>(2 * order->size()));  // cycle?
      }
      DCHECK(links > 0);
      DCHECK(links == end->rpo_number() - header->rpo_number());
      DCHECK(end_found);

      // Check loop depth of the header.
      int loop_depth = 0;
      for (LoopInfo* outer = loop; outer != NULL; outer = outer->prev) {
        loop_depth++;
      }
      DCHECK_EQ(loop_depth, header->loop_depth());

      // Check the contiguousness of loops.
      int count = 0;
      for (int j = 0; j < static_cast<int>(order->size()); j++) {
        BasicBlock* block = order->at(j);
        DCHECK(block->rpo_number() == j);
        if (j < header->rpo_number() || j >= end->rpo_number()) {
          DCHECK(!header->LoopContains(block));
        } else {
          DCHECK(header->LoopContains(block));
          DCHECK_GE(block->loop_depth(), loop_depth);
          count++;
        }
      }
      DCHECK(links == count);
    }
  }
#endif  // DEBUG

  Zone* zone_;
  Schedule* schedule_;
  BasicBlock* order_;
  BasicBlock* beyond_end_;
  ZoneVector<LoopInfo> loops_;
  ZoneVector<Backedge> backedges_;
  ZoneVector<SpecialRPOStackFrame> stack_;
  size_t previous_block_count_;
};


BasicBlockVector* Scheduler::ComputeSpecialRPO(Zone* zone, Schedule* schedule) {
  SpecialRPONumberer numberer(zone, schedule);
  numberer.ComputeSpecialRPO();
  numberer.SerializeRPOIntoSchedule();
  numberer.PrintAndVerifySpecialRPO();
  return schedule->rpo_order();
}


void Scheduler::ComputeSpecialRPONumbering() {
  Trace("--- COMPUTING SPECIAL RPO ----------------------------------\n");

  // Compute the special reverse-post-order for basic blocks.
  special_rpo_ = new (zone_) SpecialRPONumberer(zone_, schedule_);
  special_rpo_->ComputeSpecialRPO();
}


void Scheduler::PropagateImmediateDominators(BasicBlock* block) {
  for (/*nop*/; block != NULL; block = block->rpo_next()) {
    BasicBlock::Predecessors::iterator pred = block->predecessors_begin();
    BasicBlock::Predecessors::iterator end = block->predecessors_end();
    DCHECK(pred != end);  // All blocks except start have predecessors.
    BasicBlock* dominator = *pred;
    // For multiple predecessors, walk up the dominator tree until a common
    // dominator is found. Visitation order guarantees that all predecessors
    // except for backwards edges have been visited.
    for (++pred; pred != end; ++pred) {
      // Don't examine backwards edges.
      if ((*pred)->dominator_depth() < 0) continue;
      dominator = GetCommonDominator(dominator, *pred);
    }
    block->set_dominator(dominator);
    block->set_dominator_depth(dominator->dominator_depth() + 1);
    // Propagate "deferredness" of the dominator.
    if (dominator->deferred()) block->set_deferred(true);
    Trace("Block B%d's idom is B%d, depth = %d\n", block->id().ToInt(),
          dominator->id().ToInt(), block->dominator_depth());
  }
}


void Scheduler::GenerateImmediateDominatorTree() {
  Trace("--- IMMEDIATE BLOCK DOMINATORS -----------------------------\n");

  // Seed start block to be the first dominator.
  schedule_->start()->set_dominator_depth(0);

  // Build the block dominator tree resulting from the above seed.
  PropagateImmediateDominators(schedule_->start()->rpo_next());
}


// -----------------------------------------------------------------------------
// Phase 3: Prepare use counts for nodes.


class PrepareUsesVisitor : public NullNodeVisitor {
 public:
  explicit PrepareUsesVisitor(Scheduler* scheduler)
      : scheduler_(scheduler), schedule_(scheduler->schedule_) {}

  void Pre(Node* node) {
    if (scheduler_->GetPlacement(node) == Scheduler::kFixed) {
      // Fixed nodes are always roots for schedule late.
      scheduler_->schedule_root_nodes_.push_back(node);
      if (!schedule_->IsScheduled(node)) {
        // Make sure root nodes are scheduled in their respective blocks.
        Trace("Scheduling fixed position node #%d:%s\n", node->id(),
              node->op()->mnemonic());
        IrOpcode::Value opcode = node->opcode();
        BasicBlock* block =
            opcode == IrOpcode::kParameter
                ? schedule_->start()
                : schedule_->block(NodeProperties::GetControlInput(node));
        DCHECK(block != NULL);
        schedule_->AddNode(block, node);
      }
    }
  }

  void PostEdge(Node* from, int index, Node* to) {
    // If the edge is from an unscheduled node, then tally it in the use count
    // for all of its inputs. The same criterion will be used in ScheduleLate
    // for decrementing use counts.
    if (!schedule_->IsScheduled(from)) {
      DCHECK_NE(Scheduler::kFixed, scheduler_->GetPlacement(from));
      scheduler_->IncrementUnscheduledUseCount(to, index, from);
    }
  }

 private:
  Scheduler* scheduler_;
  Schedule* schedule_;
};


void Scheduler::PrepareUses() {
  Trace("--- PREPARE USES -------------------------------------------\n");

  // Count the uses of every node, it will be used to ensure that all of a
  // node's uses are scheduled before the node itself.
  PrepareUsesVisitor prepare_uses(this);
  graph_->VisitNodeInputsFromEnd(&prepare_uses);
}


// -----------------------------------------------------------------------------
// Phase 4: Schedule nodes early.


class ScheduleEarlyNodeVisitor {
 public:
  ScheduleEarlyNodeVisitor(Zone* zone, Scheduler* scheduler)
      : scheduler_(scheduler), schedule_(scheduler->schedule_), queue_(zone) {}

  // Run the schedule early algorithm on a set of fixed root nodes.
  void Run(NodeVector* roots) {
    for (Node* const root : *roots) {
      queue_.push(root);
      while (!queue_.empty()) {
        VisitNode(queue_.front());
        queue_.pop();
      }
    }
  }

 private:
  // Visits one node from the queue and propagates its current schedule early
  // position to all uses. This in turn might push more nodes onto the queue.
  void VisitNode(Node* node) {
    Scheduler::SchedulerData* data = scheduler_->GetData(node);

    // Fixed nodes already know their schedule early position.
    if (scheduler_->GetPlacement(node) == Scheduler::kFixed) {
      data->minimum_block_ = schedule_->block(node);
      Trace("Fixing #%d:%s minimum_block = B%d, dominator_depth = %d\n",
            node->id(), node->op()->mnemonic(),
            data->minimum_block_->id().ToInt(),
            data->minimum_block_->dominator_depth());
    }

    // No need to propagate unconstrained schedule early positions.
    if (data->minimum_block_ == schedule_->start()) return;

    // Propagate schedule early position.
    DCHECK(data->minimum_block_ != NULL);
    for (auto use : node->uses()) {
      PropagateMinimumPositionToNode(data->minimum_block_, use);
    }
  }

  // Propagates {block} as another minimum position into the given {node}. This
  // has the net effect of computing the minimum dominator block of {node} that
  // still post-dominates all inputs to {node} when the queue is processed.
  void PropagateMinimumPositionToNode(BasicBlock* block, Node* node) {
    Scheduler::SchedulerData* data = scheduler_->GetData(node);

    // No need to propagate to fixed node, it's guaranteed to be a root.
    if (scheduler_->GetPlacement(node) == Scheduler::kFixed) return;

    // Coupled nodes influence schedule early position of their control.
    if (scheduler_->GetPlacement(node) == Scheduler::kCoupled) {
      Node* control = NodeProperties::GetControlInput(node);
      PropagateMinimumPositionToNode(block, control);
    }

    // Propagate new position if it is deeper down the dominator tree than the
    // current. Note that all inputs need to have minimum block position inside
    // the dominator chain of {node}'s minimum block position.
    DCHECK(InsideSameDominatorChain(block, data->minimum_block_));
    if (block->dominator_depth() > data->minimum_block_->dominator_depth()) {
      data->minimum_block_ = block;
      queue_.push(node);
      Trace("Propagating #%d:%s minimum_block = B%d, dominator_depth = %d\n",
            node->id(), node->op()->mnemonic(),
            data->minimum_block_->id().ToInt(),
            data->minimum_block_->dominator_depth());
    }
  }

#if DEBUG
  bool InsideSameDominatorChain(BasicBlock* b1, BasicBlock* b2) {
    BasicBlock* dominator = scheduler_->GetCommonDominator(b1, b2);
    return dominator == b1 || dominator == b2;
  }
#endif

  Scheduler* scheduler_;
  Schedule* schedule_;
  ZoneQueue<Node*> queue_;
};


void Scheduler::ScheduleEarly() {
  Trace("--- SCHEDULE EARLY -----------------------------------------\n");
  if (FLAG_trace_turbo_scheduler) {
    Trace("roots: ");
    for (Node* node : schedule_root_nodes_) {
      Trace("#%d:%s ", node->id(), node->op()->mnemonic());
    }
    Trace("\n");
  }

  // Compute the minimum block for each node thereby determining the earliest
  // position each node could be placed within a valid schedule.
  ScheduleEarlyNodeVisitor schedule_early_visitor(zone_, this);
  schedule_early_visitor.Run(&schedule_root_nodes_);
}


// -----------------------------------------------------------------------------
// Phase 5: Schedule nodes late.


class ScheduleLateNodeVisitor {
 public:
  ScheduleLateNodeVisitor(Zone* zone, Scheduler* scheduler)
      : scheduler_(scheduler), schedule_(scheduler_->schedule_) {}

  // Run the schedule late algorithm on a set of fixed root nodes.
  void Run(NodeVector* roots) {
    for (Node* const root : *roots) {
      ProcessQueue(root);
    }
  }

 private:
  void ProcessQueue(Node* root) {
    ZoneQueue<Node*>* queue = &(scheduler_->schedule_queue_);
    for (Node* node : root->inputs()) {
      // Don't schedule coupled nodes on their own.
      if (scheduler_->GetPlacement(node) == Scheduler::kCoupled) {
        node = NodeProperties::GetControlInput(node);
      }

      // Test schedulability condition by looking at unscheduled use count.
      if (scheduler_->GetData(node)->unscheduled_count_ != 0) continue;

      queue->push(node);
      while (!queue->empty()) {
        VisitNode(queue->front());
        queue->pop();
      }
    }
  }

  // Visits one node from the queue of schedulable nodes and determines its
  // schedule late position. Also hoists nodes out of loops to find a more
  // optimal scheduling position.
  void VisitNode(Node* node) {
    DCHECK_EQ(0, scheduler_->GetData(node)->unscheduled_count_);

    // Don't schedule nodes that are already scheduled.
    if (schedule_->IsScheduled(node)) return;
    DCHECK_EQ(Scheduler::kSchedulable, scheduler_->GetPlacement(node));

    // Determine the dominating block for all of the uses of this node. It is
    // the latest block that this node can be scheduled in.
    Trace("Scheduling #%d:%s\n", node->id(), node->op()->mnemonic());
    BasicBlock* block = GetCommonDominatorOfUses(node);
    DCHECK_NOT_NULL(block);

    // The schedule early block dominates the schedule late block.
    BasicBlock* min_block = scheduler_->GetData(node)->minimum_block_;
    DCHECK_EQ(min_block, scheduler_->GetCommonDominator(block, min_block));
    Trace("Schedule late of #%d:%s is B%d at loop depth %d, minimum = B%d\n",
          node->id(), node->op()->mnemonic(), block->id().ToInt(),
          block->loop_depth(), min_block->id().ToInt());

    // Hoist nodes out of loops if possible. Nodes can be hoisted iteratively
    // into enclosing loop pre-headers until they would preceed their schedule
    // early position.
    BasicBlock* hoist_block = GetPreHeader(block);
    while (hoist_block != NULL &&
           hoist_block->dominator_depth() >= min_block->dominator_depth()) {
      Trace("  hoisting #%d:%s to block B%d\n", node->id(),
            node->op()->mnemonic(), hoist_block->id().ToInt());
      DCHECK_LT(hoist_block->loop_depth(), block->loop_depth());
      block = hoist_block;
      hoist_block = GetPreHeader(hoist_block);
    }

    // Schedule the node or a floating control structure.
    if (NodeProperties::IsControl(node)) {
      ScheduleFloatingControl(block, node);
    } else {
      ScheduleNode(block, node);
    }
  }

  BasicBlock* GetPreHeader(BasicBlock* block) {
    if (block->IsLoopHeader()) {
      return block->dominator();
    } else if (block->loop_header() != NULL) {
      return block->loop_header()->dominator();
    } else {
      return NULL;
    }
  }

  BasicBlock* GetCommonDominatorOfUses(Node* node) {
    BasicBlock* block = NULL;
    for (Edge edge : node->use_edges()) {
      BasicBlock* use_block = GetBlockForUse(edge);
      block = block == NULL ? use_block : use_block == NULL
                                              ? block
                                              : scheduler_->GetCommonDominator(
                                                    block, use_block);
    }
    return block;
  }

  BasicBlock* GetBlockForUse(Edge edge) {
    Node* use = edge.from();
    IrOpcode::Value opcode = use->opcode();
    if (opcode == IrOpcode::kPhi || opcode == IrOpcode::kEffectPhi) {
      // If the use is from a coupled (i.e. floating) phi, compute the common
      // dominator of its uses. This will not recurse more than one level.
      if (scheduler_->GetPlacement(use) == Scheduler::kCoupled) {
        Trace("  inspecting uses of coupled #%d:%s\n", use->id(),
              use->op()->mnemonic());
        DCHECK_EQ(edge.to(), NodeProperties::GetControlInput(use));
        return GetCommonDominatorOfUses(use);
      }
      // If the use is from a fixed (i.e. non-floating) phi, use the block
      // of the corresponding control input to the merge.
      if (scheduler_->GetPlacement(use) == Scheduler::kFixed) {
        Trace("  input@%d into a fixed phi #%d:%s\n", edge.index(), use->id(),
              use->op()->mnemonic());
        Node* merge = NodeProperties::GetControlInput(use, 0);
        opcode = merge->opcode();
        DCHECK(opcode == IrOpcode::kMerge || opcode == IrOpcode::kLoop);
        use = NodeProperties::GetControlInput(merge, edge.index());
      }
    }
    BasicBlock* result = schedule_->block(use);
    if (result == NULL) return NULL;
    Trace("  must dominate use #%d:%s in B%d\n", use->id(),
          use->op()->mnemonic(), result->id().ToInt());
    return result;
  }

  void ScheduleFloatingControl(BasicBlock* block, Node* node) {
    scheduler_->FuseFloatingControl(block, node);
  }

  void ScheduleNode(BasicBlock* block, Node* node) {
    schedule_->PlanNode(block, node);
    scheduler_->scheduled_nodes_[block->id().ToSize()].push_back(node);
    scheduler_->UpdatePlacement(node, Scheduler::kScheduled);
  }

  Scheduler* scheduler_;
  Schedule* schedule_;
};


void Scheduler::ScheduleLate() {
  Trace("--- SCHEDULE LATE ------------------------------------------\n");
  if (FLAG_trace_turbo_scheduler) {
    Trace("roots: ");
    for (Node* node : schedule_root_nodes_) {
      Trace("#%d:%s ", node->id(), node->op()->mnemonic());
    }
    Trace("\n");
  }

  // Schedule: Places nodes in dominator block of all their uses.
  ScheduleLateNodeVisitor schedule_late_visitor(zone_, this);
  schedule_late_visitor.Run(&schedule_root_nodes_);
}


// -----------------------------------------------------------------------------
// Phase 6: Seal the final schedule.


void Scheduler::SealFinalSchedule() {
  Trace("--- SEAL FINAL SCHEDULE ------------------------------------\n");

  // Serialize the assembly order and reverse-post-order numbering.
  special_rpo_->SerializeRPOIntoSchedule();
  special_rpo_->PrintAndVerifySpecialRPO();

  // Add collected nodes for basic blocks to their blocks in the right order.
  int block_num = 0;
  for (NodeVector& nodes : scheduled_nodes_) {
    BasicBlock::Id id = BasicBlock::Id::FromInt(block_num++);
    BasicBlock* block = schedule_->GetBlockById(id);
    for (auto i = nodes.rbegin(); i != nodes.rend(); ++i) {
      schedule_->AddNode(block, *i);
    }
  }
}


// -----------------------------------------------------------------------------


void Scheduler::FuseFloatingControl(BasicBlock* block, Node* node) {
  Trace("--- FUSE FLOATING CONTROL ----------------------------------\n");
  if (FLAG_trace_turbo_scheduler) {
    OFStream os(stdout);
    os << "Schedule before control flow fusion:\n" << *schedule_;
  }

  // Iterate on phase 1: Build control-flow graph.
  control_flow_builder_->Run(block, node);

  // Iterate on phase 2: Compute special RPO and dominator tree.
  special_rpo_->UpdateSpecialRPO(block, schedule_->block(node));
  // TODO(mstarzinger): Currently "iterate on" means "re-run". Fix that.
  for (BasicBlock* b = block->rpo_next(); b != NULL; b = b->rpo_next()) {
    b->set_dominator_depth(-1);
    b->set_dominator(NULL);
  }
  PropagateImmediateDominators(block->rpo_next());

  // Iterate on phase 4: Schedule nodes early.
  // TODO(mstarzinger): The following loop gathering the propagation roots is a
  // temporary solution and should be merged into the rest of the scheduler as
  // soon as the approach settled for all floating loops.
  NodeVector propagation_roots(control_flow_builder_->control_);
  for (Node* node : control_flow_builder_->control_) {
    for (Node* use : node->uses()) {
      if (use->opcode() == IrOpcode::kPhi ||
          use->opcode() == IrOpcode::kEffectPhi) {
        propagation_roots.push_back(use);
      }
    }
  }
  if (FLAG_trace_turbo_scheduler) {
    Trace("propagation roots: ");
    for (Node* node : propagation_roots) {
      Trace("#%d:%s ", node->id(), node->op()->mnemonic());
    }
    Trace("\n");
  }
  ScheduleEarlyNodeVisitor schedule_early_visitor(zone_, this);
  schedule_early_visitor.Run(&propagation_roots);

  // Move previously planned nodes.
  // TODO(mstarzinger): Improve that by supporting bulk moves.
  scheduled_nodes_.resize(schedule_->BasicBlockCount(), NodeVector(zone_));
  MovePlannedNodes(block, schedule_->block(node));

  if (FLAG_trace_turbo_scheduler) {
    OFStream os(stdout);
    os << "Schedule after control flow fusion:\n" << *schedule_;
  }
}


void Scheduler::MovePlannedNodes(BasicBlock* from, BasicBlock* to) {
  Trace("Move planned nodes from B%d to B%d\n", from->id().ToInt(),
        to->id().ToInt());
  NodeVector* nodes = &(scheduled_nodes_[from->id().ToSize()]);
  for (Node* const node : *nodes) {
    schedule_->SetBlockForNode(to, node);
    scheduled_nodes_[to->id().ToSize()].push_back(node);
  }
  nodes->clear();
}

}  // namespace compiler
}  // namespace internal
}  // namespace v8
