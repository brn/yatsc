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

#define PREFIX                                                        \
  template <typename SemanticAction, typename ParseResult, typename ScannerConnector> \
  ParseResult BareParser<SemanticAction, ParseResult, ScannerConnector>

namespace yatsc {

// Statement[Yield, Return]
//   BlockStatement[?Yield, ?Return]
//   VariableStatement[?Yield]
//   EmptyStatement
//   ExpressionStatement[?Yield]
//   IfStatement[?Yield, ?Return]
//   BreakableStatement[?Yield, ?Return]
//   ContinueStatement[?Yield]
//   BreakStatement[?Yield]
//   [+Return] ReturnStatement[?Yield]
//   WithStatement[?Yield, ?Return]
//   LabelledStatement[?Yield, ?Return]
//   ThrowStatement[?Yield]
//   TryStatement[?Yield, ?Return]
//   DebuggerStatement
//
// Parse statments.
// PREFIX::ParseStatement() {
//   switch (cur_token()->type()) {
//     case TokenKind::kLeftBrace:
//       return ParseBlockStatement();
//       break;

//     case TokenKind::kLineTerminator:
//       return ParseEmptyStatement();
//       break;

//     case TokenKind::kIf:
//       return ParseIfStatement();
//       break;

//     case TokenKind::kFor:
//       return ParseForStatement();
//       break;

//     case TokenKind::kWhile:
//       return ParseWhileStatement();
//       break;

//     case TokenKind::kDo:
//       return ParseDoWhileStatement();
//       break;

//     case TokenKind::kContinue:
//       // If this stament not in the iteration statement body,
//       // record error and continue parsing.
//       if (!parser_state_.IsContinuable()) {
//         sem_action_.ReportParseError(cur_token(), YATSC_SOURCEINFO_ARGS)
//           << "'continue' only allowed in loops";
//       }
//       return ParseContinueStatement();
//       break;

//     case TokenKind::kBreak: {
//       // If this stament not in the iteration statement body,
//       // record error and continue parsing.
//       if (!parser_state_.IsBreakable()) {
//         sem_action_.ReportParseError(cur_token(), YATSC_SOURCEINFO_ARGS)
//           << "'break' not allowed here";
//       }
//       auto ret = ParseBreakStatement();
      
//       if (IsLineTermination()) {
//         ConsumeLineTerminator();
//       } else if (!cur_token()->OneOf({TokenKind::kRightBrace, TokenKind::kLeftBrace})){
//         // After break statement, followed token is not ';' or line break or '}' or '{',
//         // that is error.
//         sem_action_.ReportParseError(cur_token(), YATSC_SOURCEINFO_ARGS) << "';' expected";
//       }
//       return ret;
//     }

//     case TokenKind::kReturn:
//       // If this stament not in the function body,
//       // record error and continue parsing.
//       if (!parser_state_.IsReturnable()) {
//         sem_action_.ReportParseError(cur_token(), YATSC_SOURCEINFO_ARGS) << "'return' statement only allowed in function";
//       }
//       return ParseReturnStatement();

//     case TokenKind::kWith:
//       return ParseWithStatement();

//     case TokenKind::kSwitch:
//       return ParseSwitchStatement();

//     case TokenKind::kThrow:
//       return ParseThrowStatement();

//     case TokenKind::kTry:
//       return ParseTryStatement();

//     case TokenKind::kDebugger:
//       return ParseDebuggerStatement();

//     case TokenKind::kVar: {
//       auto variable_stmt_result = ParseVariableStatement();
      
//       if (IsLineTermination()) {
//         ConsumeLineTerminator();
//       } else if (!cur_token()->OneOf({TokenKind::kRightBrace, TokenKind::kLeftBrace})) {
//         // After variable statement, followed token is not ';' or line break or '}' or '{',
//         // that is error.
//         sem_action_.ReportParseError(cur_token(), YATSC_SOURCEINFO_ARGS) << "';' expected";
//       }
//       return variable_stmt_result;
//     }

//     case TokenKind::kEof: {
//       sem_action_.UnexpectedEndOfInput(cur_token(), YATSC_SOURCEINFO_ARGS);
//       return sem_action_.Failed();
//     }
      
//     default: {
//       // If keyword encounted, record as error and,
//       // treat it as identifier and continue parsing.
//       if (Token::IsKeyword(cur_token()->type()) &&
//           !cur_token()->IsPrimaryKeyword()) {
//         sem_action_.ReportParseError(cur_token(), YATSC_SOURCEINFO_ARGS)
//           << "'" << cur_token()->utf8_value() << "' is not allowed here.";
//         cur_token()->set_type(TokenKind::kIdentifier);
//       }

//       auto tokens = token_pack();

//       // Labelled statment and expression is ambiguous,
//       // so first, try parse as a labelled statement.
//       // If labelled statement parsing is failed, parse as an expression.
//       if (cur_token()->Is(TokenKind::kIdentifier)) {
//         Next();

//         // If colon is encounted after identifier,
//         // it must be a labelled statement.
//         if (cur_token()->Is(TokenKind::kColon)) {
//           sem_action_.RestoreTokens(tokens);
//           return ParseLabelledStatement();
//         }

//         RestoreTokens(tokens);
//       }

//       // Parse an expression.
//       Token info = *cur_token();
//       auto expr = ParseExpression();
//       if (!sem_action_.CheckResult(parse_result)) {
//         return sem_action_.Failed();
//       }
//       ResultDescriptor<ir::StatementView> desc(expr);
//       return sem_action_.Success(desc);
//     }
//   }
// }


// PREFIX::ParseDeclaration(bool error) {
//   switch (cur_token()->type()) {
//     case TokenKind::kFunction: {
//       auto function_overloads_result = ParseFunctionOverloads(true, false);
//       if (!sem_action_.CheckResult(function_overloads_result)) {
//         return sem_action_.Failed();
//       }
//       return sem_action_.Success(function_overloads_result);
//     }
//     case TokenKind::kClass:
//       return ParseClassDeclaration();
//     case TokenKind::kEnum:
//       return ParseEnumDeclaration();
//     case TokenKind::kInterface:
//       return ParseInterfaceDeclaration();
//     case TokenKind::kLet:
//     case TokenKind::kConst: {
//       auto lexical_decl_result = ParseLexicalDeclaration();
//       if (IsLineTermination()) {
//         ConsumeLineTerminator();
//       } else if (!cur_token()->Is(TokenKind::kRightBrace)) {
//         sem_action_.ReportParseError(cur_token(), YATSC_SOURCEINFO_ARGS) << "';' expected.";
//       }
//       return sem_action_.Success(lexical_decl_result);
//     }
//     default:
//       if (!error) {
//         return sem_action_.Failed();
//       }
//       sem_action_.ReportParseError(cur_token(), YATSC_SOURCEINFO_ARGS) << "unexpected token.";
//       return sem_action_.Failed();
//   }
// }


// // { StatementList[?Yield, ?Return](opt) }
// //
// // Parse block statment.
// // e.g. { ... }
// PREFIX::ParseBlockStatement() {
//   sem_action_.EnterBlock();
  
//   ResultDescriptor<ir::BlockView> desc;

//   if (cur_token()->Is(TokenKind::kLeftBrace)) {
//     OpenBraceFound();
//     Next();
    
//     while (!cur_token()->Is(TokenKind::kRightBrace)) {
//       CheckEof(YATSC_SOURCEINFO_ARGS);
//       auto statement_list_result = ParseStatementListItem();
//       // If parse failed,
//       // skip to next statement beggining.
//       if (!sem_action_.CheckResult(statement_list_result)) {
//         SkipToNextStatement();
//       } else {
//         desc.Append(statement_list_result);
//       }
//     }

//     CloseBraceFound();
//     BalanceEnclosureIfNotBalanced(cur_token(), TokenKind::kRightBrace, true);

//     sem_action_.ExitBlock();
//     return sem_action_.Success(desc);
//   }
  
//   sem_action_.ReportParseError(cur_token(), YATSC_SOURCEINFO_ARGS) << "'{' expected";
//   return sem_action_.Failed();
// }


// // StatementListItem[Yield, Return]
// //   Statement[?Yield, ?Return]
// //   Declaration[?Yield]
// //
// // Parse statements and declarations.
// PREFIX::ParseStatementListItem() {
//   // First parse declarations,
//   // If declaration parsing failed,
//   // parse statement.
//   auto decl_result = ParseDeclaration(false);
  
//   if (!sem_action_.CheckResult(decl_result)) {
//     return ParseStatement();
//   }
  
//   return decl_result;
// }


// // LexicalDeclaration[In, Yield]
// //   LetOrConst BindingList[?In, ?Yield] ;
// //
// // Parse lexical declaration.
// PREFIX::ParseLexicalDeclaration() {
//   bool has_const = cur_token()->Is(TokenKind::kConst);

//   // Consume let or const.
//   Next();

//   ResultDescriptor<ir::LexicalDeclView> desc;
  
//   while (1) {
//     auto lexical_decl_result = ParseLexicalBinding(has_const);
    
//     if (sem_action_.CheckResult(lexical_decl_result)) {
//       desc.Append(lexical_decl_result);
//     } else {
//       // If parse failed, skip to comma or line end.
//       SKIP_IF_ERROR_RECOVERY_ENABLED(false, TokenKind::kComma, TokenKind::kLineTerminator);
//     }

//     // Parse comma.
//     // const a = 1, b = 2, c = 3;
//     if (cur_token()->Is(TokenKind::kComma)) {
//       Next();
//     } else {
//       break;
//     }
//   }
  
//   return sem_action_.Success(lexical_decl);
// }

}
