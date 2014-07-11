### <a name="Statement"> Statement[Yield, Return]
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

### <a name="Declaration"> Declaration[Yield, Default]
- [FunctionDeclaration\[?Yield,?Default\]](#FunctionDeclaration)
- [GeneratorDeclaration\[?Yield, ?Default\]](#GeneratorDeclaration)
- [ClassDeclaration\[?Yield,?Default\]](#ClassDeclaration)
- [LexicalDeclaration\[In, ?Yield\]](#LexicalDeclaration)

### <a name="BreakableStatement"> BreakableStatement[Yield, Return]
- [IterationStatement\[?Yield, ?Return\]](#IterationStatement)
- [SwitchStatement\[?Yield, ?Return\]](#SwitchStatement)


### <a name="BlockStatement"> BlockStatement[Yield, Return]
- [Block\[?Yield, ?Return\]](#Block)

### <a name="Block"> Block[Yield, Return]
- ___{___ [StatementList\[?Yield, ?Return\](opt)](#StatementList) ___}___

### <a name="StatementList"> StatementList[Yield, Return]
- [StatementListItem\[?Yield, ?Return\]](#StatementListItem)
- [StatementList\[?Yield, ?Return\]](#StatementList) [StatementListItem\[?Yield, ?Return\]](#StatementListItem)

### <a name="StatementListItem"> StatementListItem[Yield, Return]
- [Statement\[?Yield, ?Return\]](#Statement) [Declaration\[?Yield\]](#Declaration)

### <a name="LexicalDeclaration"> LexicalDeclaration[In, Yield]
- [LetOrConst](#LetOrConst) [BindingList\[?In, ?Yield\]](#BindingList) ___;___

### <a name="LetOrConst"> LetOrConst
- ___let___
- ___const___

### <a name="BindingList"> BindingList[In, Yield]
- [LexicalBinding\[?In, ?Yield\]](#LexicalBinding)
- [BindingList\[?In, ?Yield\]](#BindingList) , [LexicalBinding\[?In, ?Yield\]](#LexicalBinding)

### <a name="LexicalBinding"> LexicalBinding[In, Yield]
- [BindingIdentifier\[?Yield\]](#BindingIdentifier) [Initializer\[?In, ?Yield\]\(opt\)](#Initializer)
- [BindingPattern\[?Yield\]](#BindingPattern) [Initializer\[?In, ?Yield\]](#Initializer)

### <a name="VariableStatement"> VariableStatement[Yield]
- ___var___ [VariableDeclarationList\[In, ?Yield\]](#VariableDeclarationList) ___;___

### <a name="VariableDeclarationList"> VariableDeclarationList[In, Yield]
- [VariableDeclaration\[?In, ?Yield\]](#VariableDeclaration)
- [VariableDeclarationList\[?In, ?Yield\]](#VariableDeclarationList) ___,___ [VariableDeclaration\[?In, ?Yield\]](#VariableDeclaration)

### <a name="VariableDeclaration"> VariableDeclaration[In, Yield]
- [BindingIdentifier\[?Yield\]](#BindingIdentifier) [Initializer\[?In, ?Yield\]\(opt\)](#Initializer)
- [BindingPattern\[Yield\]](#BindingPattern) [Initializer\[?In, ?Yield\]](#Initializer)

### <a name="BindingPattern"> BindingPattern[Yield,GeneratorParameter]
- [ObjectBindingPattern\[?Yield,?GeneratorParameter\]](#ObjectBindingPattern)
- [ArrayBindingPattern\[?Yield,?GeneratorParameter\]](#ArrayBindingPattern)

### <a name="ObjectBindingPattern"> ObjectBindingPattern[Yield,GeneratorParameter]
- ___{___ ___}___
- ___{___ [BindingPropertyList\[?Yield,?GeneratorParameter\]](#BindingPropertyList) ___}___
- ___{___ [BindingPropertyList\[?Yield,?GeneratorParameter\]](#BindingPropertyList) ___,___ ___}___

### <a name="ArrayBindingPattern"> ArrayBindingPattern[Yield,GeneratorParameter]
- ___\[___ [Elision\(opt\)](#Elision) [BindingRestElement\[?Yield, ?GeneratorParameter\]\(opt\)](#BindingRestElement) ___\]___
- ___\[___ [BindingElementList\[?Yield, ?GeneratorParameter\]](#BindingElementList) ___\]___
- ___\[___ [BindingElementList\[?Yield, ?GeneratorParameter\]](#BindingElementList) ___,___ [Elision\(opt\)](#Elision) [BindingRestElement\[?Yield, ?GeneratorParameter\]\(opt\)](#BindingRestElement) ___\]___

### <a name="BindingPropertyList"> BindingPropertyList[Yield,GeneratorParameter]
- [BindingProperty\[?Yield, ?GeneratorParameter\]](#BindingProperty)
- [BindingPropertyList\[?Yield, ?GeneratorParameter\]](#BindingPropertyList) ___,___ [BindingProperty\[?Yield, ?GeneratorParameter\]](#BindingProperty)

### <a name="BindingElementList"> BindingElementList[Yield,GeneratorParameter]
- [BindingElisionElement\[?Yield, ?GeneratorParameter\]](#BindingElisionElement)
- [BindingElementList\[?Yield, ?GeneratorParameter\]](#BindingElementList) ___,___ [BindingElisionElement\[?Yield, ?GeneratorParameter\]](#BindingElisionElement)

### <a name="BindingElisionElement"> BindingElisionElement[Yield,GeneratorParameter]
- [Elision\(opt\)](#Elision) [BindingElement\[?Yield, ?GeneratorParameter\]](#BindingElement)

### <a name="BindingProperty"> BindingProperty[Yield,GeneratorParameter]
- [SingleNameBinding\[?Yield, ?GeneratorParameter\]](#SingleNameBinding)

### <a name="PropertyName"> PropertyName[?Yield, ?GeneratorParameter]
- [BindingElement\[?Yield, ?GeneratorParameter\]](#BindingElement)

### <a name="BindingElement"> BindingElement[Yield, GeneratorParameter ]
- [SingleNameBinding\[?Yield, ?GeneratorParameter\]](#SingleNameBinding)
- \[+GeneratorParameter\] [BindingPattern\[?Yield,GeneratorParameter\]](#BindingPattern) [Initializer\[In\]\(opt\)](#Initializer)
- \[~GeneratorParameter\] [BindingPattern\[?Yield\]](#BindingPattern) [Initializer\[In, ?Yield\]\(opt\)](#Initializer)

### <a name="SingleNameBinding"> SingleNameBinding[Yield,GeneratorParameter]
- \[+GeneratorParameter\] [BindingIdentifier\[Yield\]](#BindingIdentifier) [Initializer\[In\]\(opt\)](#Initializer)
- \[~GeneratorParameter\] [BindingIdentifier\[?Yield\]](#BindingIdentifier) [Initializer\[In, ?Yield\]\(opt\)](#Initializer)

### <a name="BindingRestElement"> BindingRestElement[Yield, GeneratorParameter]
- \[+GeneratorParameter\] ___...___ [BindingIdentifier\[Yield\]](#BindingIdentifier)
- \[~GeneratorParameter\] ___...___ [BindingIdentifier\[?Yield\]](#BindingIdentifier)

### <a name="EmptyStatement"> EmptyStatement
- ___;___

### <a name="ExpressionStatement"> ExpressionStatement[Yield]
- \[lookahead  {___{___, ___function___, ___class___, ___let___ ___\[___ }\] [Expression\[In, ?Yield\]](#Expression) ___;___

### <a name="IfStatement"> IfStatement[Yield, Return]
- ___if___ ___(___ \[Expression\\[In, ?Yield\\]\](#Expression) ___)___ \[Statement\\[?Yield, ?Return\\]\](#Statement) ___else___ \[Statement\\[?Yield, ?Return\\]\](#Statement)
- ___if___ ___(___ \[Expression\\[In, ?Yield\\]\](#Expression) ___)___ \[Statement\\[?Yield, ?Return\\]\](#Statement)

### <a name="IterationStatement"> IterationStatement[Yield, Return]
- ___do___ [Statement\[?Yield, ?Return\]](#Statement) ___while___ ___(___ [Expression\[In, ?Yield\]](#Expression) ___)___ ___;___\(opt\)
- ___while___ ___(___ [Expression\[In, ?Yield\]](#Expression) ___)___ [Statement\[?Yield, ?Return\]](#Statement)
- ___for___ ___(___ \[lookahead  {___let___ ___\[___ }\] [Expression\[?Yield\]\(opt\)](#Expression) ___;___ [Expression\[In, ?Yield\]\(opt\)](#Expression) ___;___ [Expression\[In, ?Yield\]\(opt\)](#Expression) ___)___

### <a name="Statement"> Statement[?Yield, ?Return]
- ___for___ ___(___ ___var___ [VariableDeclarationList\[?Yield\]](#VariableDeclarationList) ___;___ [Expression\[In, ?Yield\]\(opt\)](#Expression) ___;___ [Expression\[In, ?Yield\]\(opt\)](#Expression) ___)___ [Statement\[?Yield, ?Return\]](#Statement)
- ___for___ ___(___ [LexicalDeclaration\[?Yield\]](#LexicalDeclaration) [Expression\[In, ?Yield\]\(opt\)](#Expression) ___;___ [Expression\[In, ?Yield\]\(opt\)](#Expression) ___)___ [Statement\[?Yield, ?Return\]](#Statement)
- ___for___ ___(___ \[lookahead  {___let___ ___\[___ }\] [LeftHandSideExpression\[?Yield\]](#LeftHandSideExpression) ___in___ [Expression\[In, ?Yield\]](#Expression) ___)___ [Statement\[?Yield, ?Return\]](#Statement)
- ___for___ ___(___ ___var___ ForBinding\[?Yield\] ___in___ Expression\[In, ?Yield\] ___)___ Statement\[?Yield, ?Return\]
- ___for___ ___(___ [ForDeclaration\[?Yield\]](#ForDeclaration) ___in___ [Expression\[In, ?Yield\]](#Expression) ___)___ [Statement\[?Yield, ?Return\]](#Statement)
- ___for___ ___(___ \[lookahead ≠ {___let___}\] [LeftHandSideExpression\[?Yield\]](#LeftHandSideExpression) ___of___ [AssignmentExpression\[In, ?Yield\]](#AssignmentExpression) ___)___ [Statement\[?Yield, ?Return\]](#Statement)
- ___for___ ___(___ ___var___ [ForBinding\[?Yield\]](#ForBinding) ___of___ [AssignmentExpression\[In, ?Yield\]](#AssignmentExpression) ___)___ [Statement\[?Yield, ?Return\]](#Statement)
- ___for___ ___(___ [ForDeclaration\[?Yield\]](#ForDeclaration) ___of___ [AssignmentExpression\[In, ?Yield\]](#AssignmentExpression) ___)___ [Statement\[?Yield, ?Return\]](#Statement)

### <a name="ForDeclaration"> ForDeclaration[Yield] :
- [LetOrConst](#LetOrConst) [ForBinding\[?Yield\]](#ForBinding)

### <a name="ContinueStatement"> ContinueStatement[Yield]
- ___continue___ ___;___
- ___continue___ \[no LineTerminator here\] [LabelIdentifier\[?Yield\]](#LabelIdentifier) ___;___

### <a name="ReturnStatement"> ReturnStatement[Yield]
- ___return___ ___;___
- ___return___ \[no LineTerminator here\] [Expression\[In, ?Yield\]](#Expression) ___;___

### <a name="WithStatement"> WithStatement[Yield, Return]
- ___with___ ___(___ [Expression\[In, ?Yield\]](#Expression) ___)___ [Statement\[?Yield, ?Return\]](#Statement)

### <a name="SwitchStatement"> SwitchStatement[Yield, Return]
- ___switch___ ___(___ [Expression\[In, ?Yield\]](#Expression) ___)___ [CaseBlock\[?Yield, ?Return\]](#CaseBlock)

### <a name="CaseBlock"> CaseBlock[Yield, Return]
- ___{___ [CaseClauses\[?Yield, ?Return\]\(opt\)](#CaseClauses) ___}___
- ___{___ [CaseClauses\[?Yield, ?Return\]\(opt\)](#CaseClauses) [DefaultClause\[?Yield, ?Return\]](#DefaultClause) [CaseClauses\[?Yield, ?Return\]\(opt\)](#CaseClauses) ___}___

### <a name="CaseClauses"> CaseClauses[Yield, Return]
- [CaseClause\[?Yield, ?Return\]](#CaseClause)
- [CaseClauses\[?Yield, ?Return\]](#CaseClauses) [CaseClause\[?Yield, ?Return\]](#CaseClause)

### <a name="CaseClause"> CaseClause[Yield, Return]
- ___case___ [Expression\[In, ?Yield\]](#Expression) ___:___ [StatementList\[?Yield, ?Return\]\(opt\)](#StatementList)

### <a name="DefaultClause"> DefaultClause[Yield, Return]
- ___default___ ___:___ [StatementList\[?Yield, ?Return\]\(opt\)](#StatementList)

### <a name="LabelledStatement"> LabelledStatement[Yield, Return]
- [LabelIdentifier\[?Yield\]](#LabelIdentifier) ___:___ [Statement\[?Yield, ?Return\]](#Statement)

### <a name="ThrowStatement"> ThrowStatement[Yield]
- ___throw___ \[no LineTerminator here\] [Expression\[In, ?Yield\]](#Expression) ___;___

### <a name="TryStatement"> TryStatement[Yield, Return]
- ___try___ [Block\[?Yield, ?Return\]](#Block) [Catch\[?Yield, ?Return\]](#Catch)
- ___try___ [Block\[?Yield, ?Return\]](#Block) [Finally\[?Yield, ?Return\]](#Finally)
- ___try___ [Block\[?Yield, ?Return\]](#Block) [Catch\[?Yield, ?Return\]](#Catch) [Finally\[?Yield, ?Return\]](#Finally)

### <a name="Catch"> Catch[Yield, Return]
- ___catch___ ___(___ [CatchParameter\[?Yield\]](#CatchParameter) ___)___ [Block\[?Yield, ?Return\]](#Block)

### <a name="Finally"> Finally[Yield, Return]
- ___finally___ [Block\[?Yield, ?Return\]](#Block)

### <a name="CatchParameter"> CatchParameter[Yield]
- [BindingIdentifier\[?Yield\]](#BindingIdentifier)
- [BindingPattern\[?Yield\]](#BindingPattern)

### <a name="DebuggerStatement"> DebuggerStatement
- ___debugger___ ___;___

### <a name="FunctionDeclaration"> FunctionDeclaration[Yield,Default]
- ___function___ [BindingIdentifier\[?Yield, ?Default\]](#BindingIdentifier) ___(___ [FormalParameters](#FormalParameters) ___)___ ___{___ [FunctionBody](#FunctionBody) ___}___

### <a name="FunctionExpression"> FunctionExpression
- ___function___ [BindingIdentifier\(opt\)](#BindingIdentifier) ___(___ [FormalParameters](#FormalParameters) ___)___ ___{___ [FunctionBody](#FunctionBody) ___}___

### <a name="StrictFormalParameters"> StrictFormalParameters[Yield, GeneratorParameter]
- [FormalParameters\[?Yield, ?GeneratorParameter\]](#FormalParameters)

### <a name="FormalParameters"> FormalParameters[Yield,GeneratorParameter]
- \[empty\]
- [FormalParameterList\[?Yield, ?GeneratorParameter\]](#FormalParameterList)

### <a name="FormalParameterList"> FormalParameterList[Yield,GeneratorParameter]
- [FunctionRestParameter\[?Yield\]](#FunctionRestParameter)
- [FormalsList\[?Yield, ?GeneratorParameter\]](#FormalsList)
- [FormalsList\[?Yield, ?GeneratorParameter\]](#FormalsList) ___,___ [FunctionRestParameter\[?Yield\]](#FunctionRestParameter)

### <a name="FormalsList"> FormalsList[Yield,GeneratorParameter]
- [FormalParameter\[?Yield, ?GeneratorParameter\]](#FormalParameter)
- [FormalsList\[?Yield, ?GeneratorParameter\]](#FormalsList) ___,___ [FormalParameter\[?Yield,?GeneratorParameter\]](#FormalParameter)

### <a name="FunctionRestParameter"> FunctionRestParameter[Yield]
- [BindingRestElement\[Yield\]](#BindingRestElement)

### <a name="FormalParameter"> FormalParameter[Yield,GeneratorParameter]
- [BindingElement\[?Yield, ?GeneratorParameter\]](#BindingElement)

### <a name="FunctionBody"> FunctionBody[Yield]
- [FunctionStatementList\[?Yield\]](#FunctionStatementList)

### <a name="FunctionStatementList"> FunctionStatementList[Yield]
- [StatementList\[?Yield, Return\]\(opt\)](#StatementList)

### <a name="ArrowFunction"> ArrowFunction[In,Yield]
- [ArrowParameters\[?Yield\]](#ArrowParameters) \[no LineTerminator here\] ___=>___ [ConciseBody\[?In\]](#ConciseBody)

### <a name="ArrowParameters"> ArrowParameters[Yield]
- [BindingIdentifier\[?Yield\]](#BindingIdentifier)
- [CoverParenthesizedExpressionAndArrowParameterList\[?Yield\]](#CoverParenthesizedExpressionAndArrowParameterList)

### <a name="ConciseBody"> ConciseBody[In]
- \[lookahead ≠ { ___{___ }\] [AssignmentExpression\[?In\]](#AssignmentExpression) ___{___ [FunctionBody](#FunctionBody) ___}___