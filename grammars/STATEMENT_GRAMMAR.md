### Statement[Yield, Return]
- [BlockStatement\[?Yield, ?Return\]](#BlockStatement)
- [VariableStatement\[?Yield\]](#VariableStatement)
- [EmptyStatement](#EmptyStatemen)
- [ExpressionStatement\[?Yield\]](#ExpressionStatement)
- [IfStatement\[?Yield, ?Return\]](#IfStatement)
- [BreakableStatement\[?Yield, ?Return\]](#BreakableStatement)
- [ContinueStatement\[?Yield\]](#ContinueStatement)
- [BreakStatement\[?Yield\]](#BreakStatement)
- \[+Return\] [ReturnStatement\[?Yield\]](#Return)
- [WithStatement\[?Yield, ?Return\]](#WithStatement)
- [LabelledStatement\[?Yield, ?Return\]](#LabelledStatement)
- [ThrowStatement\[?Yield\]](#ThrowStatement)
- [TryStatement\[?Yield, ?Return\]](#TryStatement)
- [DebuggerStatement](#DebuggerStatemen)

### Declaration[Yield, Default]
- [FunctionDeclaration\[?Yield,?Default\]](#FunctionDeclaration)
- [GeneratorDeclaration\[?Yield, ?Default\]](#GeneratorDeclaration)
- [ClassDeclaration\[?Yield,?Default\]](#ClassDeclaration)
- [LexicalDeclaration\[In, ?Yield\]](#LexicalDeclaration)

### BreakableStatement[Yield, Return]
- [IterationStatement\[?Yield, ?Return\]](#IterationStatement)
- [SwitchStatement\[?Yield, ?Return\]](#SwitchStatement)


### BlockStatement[Yield, Return]
- [Block\[?Yield, ?Return\]](#Block)

### Block[Yield, Return]
- ___{___ [StatementList\[?Yield, ?Return\](opt)](#StatementList) ___}___

### StatementList[Yield, Return]
- [StatementListItem\[?Yield, ?Return\]](#StatementListItem)
- [StatementList\[?Yield, ?Return\]](#StatementList) [StatementListItem\[?Yield, ?Return\]](#StatementListItem)

### StatementListItem[Yield, Return]
- [Statement\[?Yield, ?Return\]](#Statement) [Declaration\[?Yield\]](#Declaration)

### LexicalDeclaration[In, Yield]
- [LetOrConst](#LetOrConst) [BindingList\[?In, ?Yield\]](#BindingList) ___;___

### LetOrConst
- ___let___
- ___const___

### BindingList[In, Yield]
- [LexicalBinding\[?In, ?Yield\]](#LexicalBinding)
- [BindingList\[?In, ?Yield\]](#BindingList) , [LexicalBinding\[?In, ?Yield\]](#LexicalBinding)

### LexicalBinding[In, Yield]
- [BindingIdentifier\[?Yield\]](#BindingIdentifier) [Initializer\[?In, ?Yield\]\(opt\)](#Initializer)
- [BindingPattern\[?Yield\]](#BindingPattern) [Initializer\[?In, ?Yield\]](#Initializer)

### VariableStatement[Yield]
- ___var___ [VariableDeclarationList\[In, ?Yield\]](#VariableDeclarationList) ___;___

### VariableDeclarationList[In, Yield]
- [VariableDeclaration\[?In, ?Yield\]](#VariableDeclaration)
- [VariableDeclarationList\[?In, ?Yield\]](#VariableDeclarationList) ___,___ [VariableDeclaration\[?In, ?Yield\]](#VariableDeclaration)

### VariableDeclaration[In, Yield]
- [BindingIdentifier\[?Yield\]](#BindingIdentifier) [Initializer\[?In, ?Yield\]\(opt\)](#Initializer)
- [BindingPattern\[Yield\]](#BindingPattern) [Initializer\[?In, ?Yield\]](#Initializer)

## BindingPattern[Yield,GeneratorParameter]
- [ObjectBindingPattern\[?Yield,?GeneratorParameter\]](#ObjectBindingPattern)
- [ArrayBindingPattern\[?Yield,?GeneratorParameter\]](#ArrayBindingPattern)

## ObjectBindingPattern[Yield,GeneratorParameter]
- ___{___ ___}___
- ___{___ [BindingPropertyList\[?Yield,?GeneratorParameter\]](#BindingPropertyList) ___}___
- ___{___ [BindingPropertyList\[?Yield,?GeneratorParameter\]](#BindingPropertyList) ___,___ ___}___

## ArrayBindingPattern[Yield,GeneratorParameter]
- ___\[___ [Elision\(opt\)](#Elision) [BindingRestElement\[?Yield, ?GeneratorParameter\]\(opt\)](#BindingRestElement) ___\]___
- ___\[___ [BindingElementList\[?Yield, ?GeneratorParameter\]](#BindingElementList) ___\]___
- ___\[___ [BindingElementList\[?Yield, ?GeneratorParameter\]](#BindingElementList) ___,___ [Elision\(opt\)](#Elision) [BindingRestElement\[?Yield, ?GeneratorParameter\]\(opt\)](#BindingRestElement) ___\]___

### BindingPropertyList[Yield,GeneratorParameter]
- [BindingProperty\[?Yield, ?GeneratorParameter\]](#BindingProperty)
- [BindingPropertyList\[?Yield, ?GeneratorParameter\]](#BindingPropertyList) ___,___ [BindingProperty\[?Yield, ?GeneratorParameter\]](#BindingProperty)

### BindingElementList[Yield,GeneratorParameter]
- [BindingElisionElement\[?Yield, ?GeneratorParameter\]](#BindingElisionElement)
- [BindingElementList\[?Yield, ?GeneratorParameter\]](#BindingElementList) ___,___ [BindingElisionElement\[?Yield, ?GeneratorParameter\]](#BindingElisionElement)

### BindingElisionElement[Yield,GeneratorParameter]
- [Elision\(opt\)](#Elision) [BindingElement\[?Yield, ?GeneratorParameter\]](#BindingElement)

### BindingProperty[Yield,GeneratorParameter]
- [SingleNameBinding\[?Yield, ?GeneratorParameter\]](#SingleNameBinding)

### PropertyName[?Yield, ?GeneratorParameter]
- [BindingElement\[?Yield, ?GeneratorParameter\]](#BindingElement)

### BindingElement[Yield, GeneratorParameter ]
- [SingleNameBinding\[?Yield, ?GeneratorParameter\]](#SingleNameBinding)
- \[+GeneratorParameter\] [BindingPattern\[?Yield,GeneratorParameter\]](#BindingPattern) [Initializer\[In\]\(opt\)](#Initializer)
- \[~GeneratorParameter\] [BindingPattern\[?Yield\]](#BindingPattern) [Initializer\[In, ?Yield\]\(opt\)](#Initializer)

### SingleNameBinding[Yield,GeneratorParameter]
- \[+GeneratorParameter\] [BindingIdentifier\[Yield\]](#BindingIdentifier) [Initializer\[In\]\(opt\)](#Initializer)
- \[~GeneratorParameter\] [BindingIdentifier\[?Yield\]](#BindingIdentifier) [Initializer\[In, ?Yield\]\(opt\)](#Initializer)

### BindingRestElement[Yield, GeneratorParameter]
- \[+GeneratorParameter\] ___...___ [BindingIdentifier\[Yield\]](#BindingIdentifier)
- \[~GeneratorParameter\] ___...___ [BindingIdentifier\[?Yield\]](#BindingIdentifier)

### EmptyStatement
- ___;___

### ExpressionStatement[Yield]
- \[lookahead  {___{___, ___function___, ___class___, ___let___ ___\[___ }\] [Expression\[In, ?Yield\]](#Expression) ___;___

### IfStatement[Yield, Return]
- ___if___ ___(___ \[Expression\\[In, ?Yield\\]\](#Expression) ___)___ \[Statement\\[?Yield, ?Return\\]\](#Statement) ___else___ \[Statement\\[?Yield, ?Return\\]\](#Statement)
- ___if___ ___(___ \[Expression\\[In, ?Yield\\]\](#Expression) ___)___ \[Statement\\[?Yield, ?Return\\]\](#Statement)

### IterationStatement[Yield, Return]
- ___do___ [Statement\[?Yield, ?Return\]](#Statement) ___while___ ___(___ [Expression\[In, ?Yield\]](#Expression) ___)___ ___;___\(opt\)
- ___while___ ___(___ [Expression\[In, ?Yield\]](#Expression) ___)___ [Statement\[?Yield, ?Return\]](#Statement)
- ___for___ ___(___ \[lookahead  {___let___ ___\[___ }\] [Expression\[?Yield\]\(opt\)](#Expression) ___;___ [Expression\[In, ?Yield\]\(opt\)](#Expression) ___;___ [Expression\[In, ?Yield\]\(opt\)](#Expression) ___)___

### Statement[?Yield, ?Return]
- ___for___ ___(___ ___var___ [VariableDeclarationList\[?Yield\]](#VariableDeclarationList) ___;___ [Expression\[In, ?Yield\]\(opt\)](#Expression) ___;___ [Expression\[In, ?Yield\]\(opt\)](#Expression) ___)___ [Statement\[?Yield, ?Return\]](#Statement)
- ___for___ ___(___ [LexicalDeclaration\[?Yield\]](#LexicalDeclaration) [Expression\[In, ?Yield\]\(opt\)](#Expression) ___;___ [Expression\[In, ?Yield\]\(opt\)](#Expression) ___)___ [Statement\[?Yield, ?Return\]](#Statement)
- ___for___ ___(___ \[lookahead  {___let___ ___\[___ }\] [LeftHandSideExpression\[?Yield\]](#LeftHandSideExpression) ___in___ [Expression\[In, ?Yield\]](#Expression) ___)___ [Statement\[?Yield, ?Return\]](#Statement)
- ___for___ ___(___ ___var___ ForBinding\[?Yield\] ___in___ Expression\[In, ?Yield\] ___)___ Statement\[?Yield, ?Return\]
- ___for___ ___(___ [ForDeclaration\[?Yield\]](#ForDeclaration) ___in___ [Expression\[In, ?Yield\]](#Expression) ___)___ [Statement\[?Yield, ?Return\]](#Statement)
- ___for___ ___(___ \[lookahead ≠ {___let___}\] [LeftHandSideExpression\[?Yield\]](#LeftHandSideExpression) ___of___ [AssignmentExpression\[In, ?Yield\]](#AssignmentExpression) ___)___ [Statement\[?Yield, ?Return\]](#Statement)
- ___for___ ___(___ ___var___ [ForBinding\[?Yield\]](#ForBinding) ___of___ [AssignmentExpression\[In, ?Yield\]](#AssignmentExpression) ___)___ [Statement\[?Yield, ?Return\]](#Statement)
- ___for___ ___(___ [ForDeclaration\[?Yield\]](#ForDeclaration) ___of___ [AssignmentExpression\[In, ?Yield\]](#AssignmentExpression) ___)___ [Statement\[?Yield, ?Return\]](#Statement)

### ForDeclaration[Yield] :
- [LetOrConst](#LetOrConst) [ForBinding\[?Yield\]](#ForBinding)

### ContinueStatement[Yield]
- ___continue___ ___;___
- ___continue___ \[no LineTerminator here\] [LabelIdentifier\[?Yield\]](#LabelIdentifier) ___;___

### ReturnStatement[Yield]
- ___return___ ___;___
- ___return___ \[no LineTerminator here\] [Expression\[In, ?Yield\]](#Expression) ___;___

### WithStatement[Yield, Return]
- ___with___ ___(___ [Expression\[In, ?Yield\]](#Expression) ___)___ [Statement\[?Yield, ?Return\]](#Statement)

SwitchStatement[Yield, Return]
- ___switch___ ___(___ [Expression\[In, ?Yield\]](#Expression) ___)___ [CaseBlock\[?Yield, ?Return\]](#CaseBlock)

### CaseBlock[Yield, Return]
- ___{___ [CaseClauses\[?Yield, ?Return\]\(opt\)](#CaseClauses) ___}___
- ___{___ [CaseClauses\[?Yield, ?Return\]\(opt\)](#CaseClauses) [DefaultClause\[?Yield, ?Return\]](#DefaultClause) [CaseClauses\[?Yield, ?Return\]\(opt\)](#CaseClauses) ___}___

### CaseClauses[Yield, Return]
- [CaseClause\[?Yield, ?Return\]](#CaseClause)
- [CaseClauses\[?Yield, ?Return\]](#CaseClauses) [CaseClause\[?Yield, ?Return\]](#CaseClause)

### CaseClause[Yield, Return]
- ___case___ [Expression\[In, ?Yield\]](#Expression) ___:___ [StatementList\[?Yield, ?Return\]\(opt\)](#StatementList)

### DefaultClause[Yield, Return]
- ___default___ ___:___ [StatementList\[?Yield, ?Return\]\(opt\)](#StatementList)

### LabelledStatement[Yield, Return]
- [LabelIdentifier\[?Yield\]](#LabelIdentifier) ___:___ [Statement\[?Yield, ?Return\]](#Statement)

### ThrowStatement[Yield]
- ___throw___ \[no LineTerminator here\] [Expression\[In, ?Yield\]](#Expression) ___;___

### TryStatement[Yield, Return]
- ___try___ [Block\[?Yield, ?Return\]](#Block) [Catch\[?Yield, ?Return\]](#Catch)
- ___try___ [Block\[?Yield, ?Return\]](#Block) [Finally\[?Yield, ?Return\]](#Finally)
- ___try___ [Block\[?Yield, ?Return\]](#Block) [Catch\[?Yield, ?Return\]](#Catch) [Finally\[?Yield, ?Return\]](#Finally)

### Catch[Yield, Return]
- ___catch___ ___(___ [CatchParameter\[?Yield\]](#CatchParameter) ___)___ [Block\[?Yield, ?Return\]](#Block)

### Finally[Yield, Return]
- ___finally___ [Block\[?Yield, ?Return\]](#Block)

### CatchParameter[Yield]
- [BindingIdentifier\[?Yield\]](#BindingIdentifier)
- [BindingPattern\[?Yield\]](#BindingPattern)

### DebuggerStatement
- ___debugger___ ___;___

### FunctionDeclaration[Yield,Default]
- ___function___ [BindingIdentifier\[?Yield, ?Default\]](#BindingIdentifier) ___(___ [FormalParameters](#FormalParameters) ___)___ ___{___ [FunctionBody](#FunctionBody) ___}___

### FunctionExpression
- ___function___ [BindingIdentifier\(opt\)](#BindingIdentifier) ___(___ [FormalParameters](#FormalParameters) ___)___ ___{___ [FunctionBody](#FunctionBody) ___}___

### StrictFormalParameters[Yield, GeneratorParameter]
- [FormalParameters\[?Yield, ?GeneratorParameter\]](#FormalParameters)

### FormalParameters[Yield,GeneratorParameter]
- \[empty\]
- [FormalParameterList\[?Yield, ?GeneratorParameter\]](#FormalParameterList)

### FormalParameterList[Yield,GeneratorParameter]
- [FunctionRestParameter\[?Yield\]](#FunctionRestParameter)
- [FormalsList\[?Yield, ?GeneratorParameter\]](#FormalsList)
- [FormalsList\[?Yield, ?GeneratorParameter\]](#FormalsList) ___,___ [FunctionRestParameter\[?Yield\]](#FunctionRestParameter)

### FormalsList[Yield,GeneratorParameter]
- [FormalParameter\[?Yield, ?GeneratorParameter\]](#FormalParameter)
- [FormalsList\[?Yield, ?GeneratorParameter\]](#FormalsList) ___,___ [FormalParameter\[?Yield,?GeneratorParameter\]](#FormalParameter)

### FunctionRestParameter[Yield]
- [BindingRestElement\[Yield\]](#BindingRestElement)

### FormalParameter[Yield,GeneratorParameter]
- [BindingElement\[?Yield, ?GeneratorParameter\]](#BindingElement)

### FunctionBody[Yield]
- [FunctionStatementList\[?Yield\]](#FunctionStatementList)

### FunctionStatementList[Yield]
- [StatementList\[?Yield, Return\]\(opt\)](#StatementList)

### ArrowFunction[In,Yield]
- \[ArrowParameters\\[?Yield\\]\](#ArrowParameters) \\[no LineTerminator here\\] ___=>___ \[ConciseBody\\[?In\\]\](#ConciseBody)

### ArrowParameters[Yield]
- [BindingIdentifier]\[?Yield\](#BindingIdentifier)
- [CoverParenthesizedExpressionAndArrowParameterList\[?Yield\]](#CoverParenthesizedExpressionAndArrowParameterList)

### ConciseBody[In]
- \[lookahead ≠ { ___{___ }\] [AssignmentExpression\[?In\]](#AssignmentExpression) ___{___ [FunctionBody](#FunctionBody) ___}___
