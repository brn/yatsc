// The MIT License (MIT)
// 
// Copyright (c) 2013 Taketoshi Aono(brn)
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#ifndef YATSC_PARSER_PARSER_STATE_H
#define YATSC_PARSER_PARSER_STATE_H

namespace yatsc {

class ParserState {
 public:
  ParserState() = default;

  enum class State: uint8_t {
    kFunction = 0,
    kGenerator,
    kIteration,
    kCaseBlock,
    kNoIn
  };

  ParserState(const ParserState& parser_state)
      : state_bit_(0),
        state_(parser_state.state_) {}


  ParserState(ParserState&& parser_state)
      : state_bit_(0),
        state_(std::move(parser_state.state_)) {}


  ParserState& operator = (ParserState parser_state) {
    if (this != &parser_state) {
      parser_state.Swap(*this);
    }
    return *this;
  }


  ParserState& operator = (ParserState&& parser_state) {
    if (this != &parser_state) {
      ParserState s(parser_state);
      s.Swap(*this);
    }
    return *this;
  }
  

  YATSC_INLINE void EnterFunction() {PushState(State::kFunction);}

  YATSC_INLINE void ExitFunction() {PopState(State::kFunction);}

  YATSC_INLINE bool IsInFunction() YATSC_NO_SE {return ExistsBefore(State::kGenerator, State::kFunction);}

  YATSC_INLINE void EnterGenerator() {PushState(State::kGenerator);}

  YATSC_INLINE void ExitGenerator() {PopState(State::kGenerator);}

  YATSC_INLINE bool IsInGenerator() YATSC_NO_SE {return ExistsBefore(State::kFunction, State::kGenerator);}

  YATSC_INLINE void EnterIteration() {PushState(State::kIteration);}

  YATSC_INLINE void ExitIteration() {PopState(State::kIteration);}

  YATSC_INLINE bool IsInIteration() YATSC_NO_SE {return IsInState(State::kIteration);}

  YATSC_INLINE void EnterCaseBlock() {PushState(State::kCaseBlock);}

  YATSC_INLINE void ExitCaseBlock() {PopState(State::kCaseBlock);}

  YATSC_INLINE void EnterNoInExpr() {PushState(State::kNoIn);}

  YATSC_INLINE void ExitNoInExpr() {PopState(State::kNoIn);}

  YATSC_INLINE void EnterErrorRecovery() {state_bit_ |= 0x1;}

  YATSC_INLINE void ExitErrorRecovery() {state_bit_ &= ~0x1;}

  YATSC_INLINE bool IsInErrorRecoveryMode() YATSC_NO_SE {return (state_bit_ & 0x1) == 0x1;}

  YATSC_INLINE void EnableEofChecker() {state_bit_ |= 0x2;}

  YATSC_INLINE void DisableEofChecker() {state_bit_ &= ~0x2;}

  YATSC_INLINE bool IsEofCheckerEnabled() YATSC_NO_SE {return (state_bit_ & 0x2) == 0x2;}

  YATSC_INLINE bool IsInNoInExpr() YATSC_NO_SE {return IsInState(State::kNoIn);}

  YATSC_INLINE bool IsInCaseBlock() YATSC_NO_SE {return IsInState(State::kCaseBlock);}

  YATSC_INLINE bool IsContinuable() YATSC_NO_SE {return IsInCaseBlock() || IsInIteration();}

  YATSC_INLINE bool IsBreakable() YATSC_NO_SE {return IsInIteration() || IsInCaseBlock();}

  YATSC_INLINE bool IsReturnable() YATSC_NO_SE {return IsInFunction() || IsInGenerator();}

  friend void swap(yatsc::ParserState& a, yatsc::ParserState& b) YATSC_NOEXCEPT {
    a.Swap(b);
  }
  
 private:

  YATSC_INLINE ParserState& Swap(ParserState& from) YATSC_NOEXCEPT {
    using std::swap;
    swap(state_, from.state_);
    swap(state_bit_, from.state_bit_);
    return *this;
  }

  YATSC_INLINE void PushState(State state) {
    state_.push_back(state);
  }

  YATSC_INLINE void PopState(State state) {
    ASSERT(state_.size() > 0, true);
    ASSERT(state_.back(), state);
    state_.pop_back();
  }

  bool ExistsBefore(State before, State target) YATSC_NO_SE {
    bool found = false;
    for (auto c: state_) {
      if (c == before && !found) {
        return false;
      } else if (c == target) {
        found = true;
      }
    }
    return found;
  }

  YATSC_INLINE bool IsInState(State state) YATSC_NO_SE {
    return state_.size() > 0 && state_.back() == state;
  }

  uint8_t state_bit_;
  Vector<State> state_;
};

}

#endif
