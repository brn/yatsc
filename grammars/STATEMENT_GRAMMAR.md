### <a name="Program"> Program[Script,Module,TsModule]
- \[+Script\] [Script](#Script)
- \[+Module\] [Module](./MODULE_SYNTAX_GRAMMAR.md#Module)
- \[+TsModule\] [ModuleDeclaration](#ModuleDeclaration)
- \[+TsModule\] ___export___ ___=___ [Identifier](./EXPRESSION_SYNTAX_GRAMMAR.md#Identifier) ___;___
- \[+TsModule\] ___export\(opt\)___ [ExternalImportDeclaration](#ExternalImportDeclaration)
- ___export\(opt\)___ [AmbientDeclaration](#AmbientDeclaration)

### <a name="SourceElements"> Script
- [ScriptBody](#ScriptBody)

### <a name="ScriptBody"> ScriptBody
- [StatementList](#StatementList)

### <a name="Statement"> Statement[Yield, Return]
- [BlockStatement\[?Yield, ?Return\]](#BlockStatement)
- [VariableStatement\[?Yield\]](#VariableStatement)
- [EmptyStatement](#EmptyStatemen)
- [ExpressionStatement\[?Yield\]](./EXPRESSION_SYNTAX_GRAMMAR.md#ExpressionStatement)
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
- [Statement\[?Yield, ?Return\]](#Statement)
- [Declaration\[?Yield\]](#Declaration)

### <a name="LexicalDeclaration"> LexicalDeclaration[In, Yield]
- [LetOrConst](#LetOrConst) [BindingList\[?In, ?Yield\]](#BindingList) ___;___

### <a name="LetOrConst"> LetOrConst
- ___let___
- ___const___

### <a name="BindingList"> BindingList[In, Yield]
- [LexicalBinding\[?In, ?Yield\]](#LexicalBinding)
- [BindingList\[?In, ?Yield\]](#BindingList) , [LexicalBinding\[?In, ?Yield\]](#LexicalBinding)

### <a name="LexicalBinding"> LexicalBinding[In, Yield]
- [BindingIdentifier\[?Yield\]](./EXPRESSION_SYNTAX_GRAMMAR.md#BindingIdentifier) [TypeAnnotation\(opt\)](#TypeAnnotation) [Initializer\[?In, ?Yield\]\(opt\)](#Initializer)
- [BindingPattern\[?Yield\]](#BindingPattern) [TypeAnnotation\(opt\)](#TypeAnnotation) [Initializer\[?In, ?Yield\]](#Initializer)

### <a name="VariableStatement"> VariableStatement[Yield]
- ___var___ [VariableDeclarationList\[In, ?Yield\]](#VariableDeclarationList) ___;___

### <a name="VariableDeclarationList"> VariableDeclarationList[In, Yield]
- [VariableDeclaration\[?In, ?Yield\]](#VariableDeclaration)
- [VariableDeclarationList\[?In, ?Yield\]](#VariableDeclarationList) ___,___ [VariableDeclaration\[?In, ?Yield\]](#VariableDeclaration)

### <a name="VariableDeclaration"> VariableDeclaration[In, Yield]
- [BindingIdentifier\[?Yield\]](./EXPRESSION_SYNTAX_GRAMMAR.md#BindingIdentifier) [TypeAnnotation\(opt\)](#TypeAnnotation) [Initializer\[?In, ?Yield\]\(opt\)](#Initializer)
- [BindingPattern\[Yield\]](#BindingPattern) [TypeAnnotation\(opt\)](#TypeAnnotation) [Initializer\[?In, ?Yield\]](#Initializer)

### <a name="TypeAnnotation"> TypeAnnotation
- ___:___ [Type](#./TYPE_SYNTAX_GRAMMAR.md#Type)
- ___:___ ___yield___ [Type](#./TYPE_SYNTAX_GRAMMAR.md#Type)

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
- \[+GeneratorParameter\] [BindingIdentifier\[Yield\]](./EXPRESSION_SYNTAX_GRAMMAR.md#BindingIdentifier) [Initializer\[In\]\(opt\)](#Initializer)
- \[~GeneratorParameter\] [BindingIdentifier\[?Yield\]](./EXPRESSION_SYNTAX_GRAMMAR.md#BindingIdentifier) [Initializer\[In, ?Yield\]\(opt\)](#Initializer)

### <a name="BindingRestElement"> BindingRestElement[Yield, GeneratorParameter]
- \[+GeneratorParameter\] ___...___ [BindingIdentifier\[Yield\]](./EXPRESSION_SYNTAX_GRAMMAR.md#BindingIdentifier)
- \[~GeneratorParameter\] ___...___ [BindingIdentifier\[?Yield\]](./EXPRESSION_SYNTAX_GRAMMAR.md#BindingIdentifier)

### <a name="EmptyStatement"> EmptyStatement
- ___;___

### <a name="ExpressionStatement"> ExpressionStatement[Yield]
- \[lookahead  {___{___, ___function___, ___class___, ___let___ ___\[___ }\] [Expression\[In, ?Yield\]](./EXPRESSION_SYNTAX_GRAMMAR.md#Expression) ___;___

### <a name="IfStatement"> IfStatement[Yield, Return]
- ___if___ ___(___ [Expression\[In, ?Yield\]](./EXPRESSION_SYNTAX_GRAMMAR.md#Expression) ___)___ [Statement\[?Yield, ?Return\]](#Statement) ___else___ [Statement\[?Yield, ?Return\]](#Statement)
- ___if___ ___(___ [Expression\[In, ?Yield\]](./EXPRESSION_SYNTAX_GRAMMAR.md#Expression) ___)___ [Statement\[?Yield, ?Return\]](#Statement)

### <a name="IterationStatement"> IterationStatement[Yield, Return]
- ___do___ [Statement\[?Yield, ?Return\]](#Statement) ___while___ ___(___ [Expression\[In, ?Yield\]](./EXPRESSION_SYNTAX_GRAMMAR.md#Expression) ___)___ ___;___\(opt\)
- ___while___ ___(___ [Expression\[In, ?Yield\]](./EXPRESSION_SYNTAX_GRAMMAR.md#Expression) ___)___ [Statement\[?Yield, ?Return\]](#Statement)
- ___for___ ___(___ \[lookahead  {___let___ ___\[___ }\] [Expression\[?Yield\]\(opt\)](./EXPRESSION_SYNTAX_GRAMMAR.md#Expression) ___;___ [Expression\[In, ?Yield\]\(opt\)](./EXPRESSION_SYNTAX_GRAMMAR.md#Expression) ___;___ [Expression\[In, ?Yield\]\(opt\)](./EXPRESSION_SYNTAX_GRAMMAR.md#Expression) ___)___

### <a name="Statement"> Statement[?Yield, ?Return]
- ___for___ ___(___ ___var___ [VariableDeclarationList\[?Yield\]](#VariableDeclarationList) ___;___ [Expression\[In, ?Yield\]\(opt\)](./EXPRESSION_SYNTAX_GRAMMAR.md#Expression) ___;___ [Expression\[In, ?Yield\]\(opt\)](./EXPRESSION_SYNTAX_GRAMMAR.md#Expression) ___)___ [Statement\[?Yield, ?Return\]](#Statement)
- ___for___ ___(___ [LexicalDeclaration\[?Yield\]](#LexicalDeclaration) [Expression\[In, ?Yield\]\(opt\)](./EXPRESSION_SYNTAX_GRAMMAR.md#Expression) ___;___ [Expression\[In, ?Yield\]\(opt\)](./EXPRESSION_SYNTAX_GRAMMAR.md#Expression) ___)___ [Statement\[?Yield, ?Return\]](#Statement)
- ___for___ ___(___ \[lookahead  {___let___ ___\[___ }\] [LeftHandSideExpression\[?Yield\]](#LeftHandSideExpression) ___in___ [Expression\[In, ?Yield\]](./EXPRESSION_SYNTAX_GRAMMAR.md#Expression) ___)___ [Statement\[?Yield, ?Return\]](#Statement)
- ___for___ ___(___ ___var___ ForBinding\[?Yield\] ___in___ Expression\[In, ?Yield\] ___)___ Statement\[?Yield, ?Return\]
- ___for___ ___(___ [ForDeclaration\[?Yield\]](#ForDeclaration) ___in___ [Expression\[In, ?Yield\]](./EXPRESSION_SYNTAX_GRAMMAR.md#Expression) ___)___ [Statement\[?Yield, ?Return\]](#Statement)
- ___for___ ___(___ \[lookahead ≠ {___let___}\] [LeftHandSideExpression\[?Yield\]](#LeftHandSideExpression) ___of___ [AssignmentExpression\[In, ?Yield\]](./EXPRESSION_SYNTAX_GRAMMAR.md#AssignmentExpression) ___)___ [Statement\[?Yield, ?Return\]](#Statement)
- ___for___ ___(___ ___var___ [ForBinding\[?Yield\]](#ForBinding) ___of___ [AssignmentExpression\[In, ?Yield\]](./EXPRESSION_SYNTAX_GRAMMAR.md#AssignmentExpression) ___)___ [Statement\[?Yield, ?Return\]](#Statement)
- ___for___ ___(___ [ForDeclaration\[?Yield\]](#ForDeclaration) ___of___ [AssignmentExpression\[In, ?Yield\]](./EXPRESSION_SYNTAX_GRAMMAR.md#AssignmentExpression) ___)___ [Statement\[?Yield, ?Return\]](#Statement)

### <a name="ForDeclaration"> ForDeclaration[Yield] :
- [LetOrConst](#LetOrConst) [ForBinding\[?Yield\]](#ForBinding)

### <a name="ContinueStatement"> ContinueStatement[Yield]
- ___continue___ ___;___
- ___continue___ \[no LineTerminator here\] [LabelIdentifier\[?Yield\]](./EXPRESSION_SYNTAX_GRAMMAR.md#LabelIdentifier) ___;___

### <a name="ReturnStatement"> ReturnStatement[Yield]
- ___return___ ___;___
- ___return___ \[no LineTerminator here\] [Expression\[In, ?Yield\]](./EXPRESSION_SYNTAX_GRAMMAR.md#Expression) ___;___

### <a name="WithStatement"> WithStatement[Yield, Return]
- ___with___ ___(___ [Expression\[In, ?Yield\]](./EXPRESSION_SYNTAX_GRAMMAR.md#Expression) ___)___ [Statement\[?Yield, ?Return\]](#Statement)

### <a name="SwitchStatement"> SwitchStatement[Yield, Return]
- ___switch___ ___(___ [Expression\[In, ?Yield\]](./EXPRESSION_SYNTAX_GRAMMAR.md#Expression) ___)___ [CaseBlock\[?Yield, ?Return\]](#CaseBlock)

### <a name="CaseBlock"> CaseBlock[Yield, Return]
- ___{___ [CaseClauses\[?Yield, ?Return\]\(opt\)](#CaseClauses) ___}___
- ___{___ [CaseClauses\[?Yield, ?Return\]\(opt\)](#CaseClauses) [DefaultClause\[?Yield, ?Return\]](#DefaultClause) [CaseClauses\[?Yield, ?Return\]\(opt\)](#CaseClauses) ___}___

### <a name="CaseClauses"> CaseClauses[Yield, Return]
- [CaseClause\[?Yield, ?Return\]](#CaseClause)
- [CaseClauses\[?Yield, ?Return\]](#CaseClauses) [CaseClause\[?Yield, ?Return\]](#CaseClause)

### <a name="CaseClause"> CaseClause[Yield, Return]
- ___case___ [Expression\[In, ?Yield\]](./EXPRESSION_SYNTAX_GRAMMAR.md#Expression) ___:___ [StatementList\[?Yield, ?Return\]\(opt\)](#StatementList)

### <a name="DefaultClause"> DefaultClause[Yield, Return]
- ___default___ ___:___ [StatementList\[?Yield, ?Return\]\(opt\)](#StatementList)

### <a name="LabelledStatement"> LabelledStatement[Yield, Return]
- [LabelIdentifier\[?Yield\]](./EXPRESSION_SYNTAX_GRAMMAR.md#LabelIdentifier) ___:___ [Statement\[?Yield, ?Return\]](#Statement)

### <a name="ThrowStatement"> ThrowStatement[Yield]
- ___throw___ \[no LineTerminator here\] [Expression\[In, ?Yield\]](./EXPRESSION_SYNTAX_GRAMMAR.md#Expression) ___;___

### <a name="TryStatement"> TryStatement[Yield, Return]
- ___try___ [Block\[?Yield, ?Return\]](#Block) [Catch\[?Yield, ?Return\]](#Catch)
- ___try___ [Block\[?Yield, ?Return\]](#Block) [Finally\[?Yield, ?Return\]](#Finally)
- ___try___ [Block\[?Yield, ?Return\]](#Block) [Catch\[?Yield, ?Return\]](#Catch) [Finally\[?Yield, ?Return\]](#Finally)

### <a name="Catch"> Catch[Yield, Return]
- ___catch___ ___(___ [CatchParameter\[?Yield\]](#CatchParameter) ___)___ [Block\[?Yield, ?Return\]](#Block)

### <a name="Finally"> Finally[Yield, Return]
- ___finally___ [Block\[?Yield, ?Return\]](#Block)

### <a name="CatchParameter"> CatchParameter[Yield]
- [BindingIdentifier\[?Yield\]](./EXPRESSION_SYNTAX_GRAMMAR.md#BindingIdentifier)
- [BindingPattern\[?Yield\]](#BindingPattern)

### <a name="DebuggerStatement"> DebuggerStatement
- ___debugger___ ___;___

### <a name="FunctionDeclaration"> FunctionDeclaration[Yield,Default]
- [FunctionOverloads\(opt\)](#FunctionOverloads) [FunctionImplementation](#FunctionImplementation)

### <a name="FunctionOverloads"> FunctionOverloads
- [FunctionOverload](#FunctionOverload)
- [FunctionOverloads](#FunctionOverloads) [FunctionOverload](#FunctionOverload)

### <a name="FunctionOverload"> FunctionOverload
- ___function___ [BindingIdentifier\[?Yield, ?Default\]](./EXPRESSION_SYNTAX_GRAMMAR.md#BindingIdentifier) [CallSignature](./TYPE_SYNTAX_GRAMMAR#CallSignature)

### <a name="FunctionImplementation"> FunctionImplementation
- ___function___ [BindingIdentifier\[?Yield, ?Default\]](./EXPRESSION_SYNTAX_GRAMMAR.md#BindingIdentifier) [CallSignature](./TYPE_SYNTAX_GRAMMAR#CallSignature) ___{___ [FunctionBody](#FunctionBody) ___}___

### <a name="FunctionExpression"> FunctionExpression
- ___function___ [BindingIdentifier\(opt\)](./EXPRESSION_SYNTAX_GRAMMAR.md#BindingIdentifier) [CallSignature](./TYPE_SYNTAX_GRAMMAR.md#CallSignature) ___{___ [FunctionBody](#FunctionBody) ___}___

### <a name="StrictFormalParameters"> StrictFormalParameters[Yield, GeneratorParameter]
- [FormalParameters\[?Yield, ?GeneratorParameter\]](#FormalParameters)

### <a name="FormalParameters"> FormalParameters[Yield,GeneratorParameter]
- \[empty\]
- [FormalParameterList\[?Yield, ?GeneratorParameter\]](#FormalParameterList)
- [RequiredParameterList\[?Yield, ?GeneratorParameter\]](#RequiredParameterList)
- [OptionalParameterList\[?Yield, ?GeneratorParameter\]](#OptionalParameterList)
- [RequiredParameterList\[?Yield, ?GeneratorParameter\]](#RequiredParameterList) ___,___ [OptionalParameterList\[?Yield, ?GeneratorParameter\]](#OptionalParameterList)
- [RequiredParameterList\[?Yield, ?GeneratorParameter\]](#RequiredParameterList) ___,___ [FunctionRestParameter\[?Yield, ?GeneratorParameter\]](./STATEMENT_GRAMMAR#FunctionRestParameter)
- [OptionalParameterList\[?Yield, ?GeneratorParameter\]](#OptionalParameterList) ___,___ [FunctionRestParameter\[?Yield, ?GeneratorParameter\]](./STATEMENT_GRAMMAR#FunctionRestParameter)
- [RequiredParameterList\[?Yield, ?GeneratorParameter\]](#RequiredParameterList) ___,___ [OptionalParameterList\[?Yield, ?GeneratorParameter\]](#OptionalParameterList) ___,___ [FunctionRestParameter\[?Yield, ?GeneratorParameter\]](./STATEMENT_GRAMMAR#FunctionRestParameter)

### <a name="OptionalParameterList"> OptionalParameterList\[Yield,GeneratorParameter\]
- [OptionalParameter\[?Yield, ?GeneratorParameter\]](#OptionalParameter)
- [OptionalParameterList\[?Yield, ?GeneratorParameter\]](#OptionalParameterList) ___,___ [OptionalParameter\[?Yield, ?GeneratorParameter\]](#OptionalParameter)

### <a name="OptionalParameter"> OptionalParameter
- [PublicOrPrivateopt](#PublicOrPrivateopt) [BindingElement\[?Yield, ?GeneratorParameter\]](#BindingElement) ___?___ [TypeAnnotation\(opt\)](#TypeAnnotation)
- [PublicOrPrivateopt](#PublicOrPrivateopt) [BindingElement\[?Yield, ?GeneratorParameter\]](#BindingElement) [TypeAnnotation\(opt\)](#TypeAnnotation) [Initialiser](#Initialiser)

### <a name="FormalParameterList"> FormalParameterList\[Yield,GeneratorParameter\]
- [FunctionRestParameter\[?Yield\]](#FunctionRestParameter)
- [FormalsList\[?Yield, ?GeneratorParameter\]](#FormalsList)
- [FormalsList\[?Yield, ?GeneratorParameter\]](#FormalsList) ___,___ [FunctionRestParameter\[?Yield\]](#FunctionRestParameter)

### <a name="FormalsList"> FormalsList[Yield,GeneratorParameter]
- [FormalParameter\[?Yield, ?GeneratorParameter\]](#FormalParameter) [TypeAnnotation](./TYPE_SYNTAX_GRAMMAR.md#TypeAnnotation)
- [FormalsList\[?Yield, ?GeneratorParameter\]](#FormalsList) ___,___ [FormalParameter\[?Yield,?GeneratorParameter\]](#FormalParameter)

### <a name="FunctionRestParameter"> FunctionRestParameter[Yield]
- [BindingRestElement\[Yield\]](#BindingRestElement) [TypeAnnotation\(opt\)](./TYPE_SYNTAX_GRAMMAR.md#TypeAnnotation)

### <a name="FormalParameter"> FormalParameter[Yield,GeneratorParameter]
- [PublicOrPrivate\(opt\)](#PublicOrPrivate)  [FormalParameterElement\[?Yield, ?GeneratorParameter\]](#FormalParameterElement) [TypeAnnotation\(opt\)](./TYPE_SYNTAX_GRAMMAR.md#TypeAnnotation)
- [Identifier](./EXPRESSION_SYNTAX_GRAMMAR#Identifier) ___:___ [StringLiteral](./EXPRESSION_SYNTAX_GRAMMAR#StringLiteral)

### <a name="FormalParameter"> FormalParameterElement[Yield,GeneratorParameter]
- [BindingIdentifier\[?Yield\]](#BindingIdentifier)]
- \[+GeneratorParameter\] [BindingPattern\[?Yield,GeneratorParameter\]](#BindingPattern)
- \[~GeneratorParameter\] [BindingPattern\[?Yield\]](#BindingPattern)

### <a name="PublicOrPrivate"> PublicOrPrivate
- ___public___
- ___private___

### <a name="FunctionBody"> FunctionBody[Yield]
- [FunctionStatementList\[?Yield\]](#FunctionStatementList)

### <a name="FunctionStatementList"> FunctionStatementList[Yield]
- [StatementList\[?Yield, Return\]\(opt\)](#StatementList)

### <a name="ArrowFunction"> ArrowFunction[In,Yield]
- [ArrowParameters\[?Yield\]](#ArrowParameters) \[no LineTerminator here\] ___=>___ [ConciseBody\[?In\]](#ConciseBody)

### <a name="ArrowParameters"> ArrowParameters[Yield]
- [BindingIdentifier\[?Yield\]](./EXPRESSION_SYNTAX_GRAMMAR.md#BindingIdentifier)
- [CallSignature](./TYPE_SYNTAX_GRAMMAR.md#CallSignature)

### <a name="ConciseBody"> ConciseBody[In]
- \[lookahead ≠ { ___{___ }\] [AssignmentExpression\[?In\]](./EXPRESSION_SYNTAX_GRAMMAR.md#AssignmentExpression) ___{___ [FunctionBody](#FunctionBody) ___}___

### <a name="GeneratorMethodDeclaration"> GeneratorMethodDeclaration[Yield]
- [GeneratorMethodOverloads\(opt\)](#GeneratorMethodOverloads) [GeneratorMethodImplementation](#GeneratorMethodImplementation)

### <a name="GeneratorMethodOverloads"> GeneratorMethodOverloads[Yield]
- [GeneratorMethodOverload](#GeneratorMethodOverload)
- [GeneratorMethodOverloads](#GeneratorMethodOverloads) [GeneratorMethodOverload](#GeneratorMethodOverload)

### <a name="GeneratorMethodOverload"> GeneratorMethodOverload
- [PublicOrPrivate\(opt\)](#PublicOrPrivate) ___*___ [PropertyName\[?Yield\]](#PropertyName) [CallSignature](#CallSignature) ___;___

### <a name="GeneratorMethodImplementation"> GeneratorMethodImplementation
- [PublicOrPrivate\(opt\)](#PublicOrPrivate) ___*___ [PropertyName\[?Yield\]](#PropertyName) [CallSignature](#CallSignature) ___{___ [FunctionBody\[Yield\]](#FunctionBody) ___}___

### <a name="GeneratorDeclaration"> GeneratorDeclaration[Yield, Default]
- ___function___ ___*___ [BindingIdentifier\[?Yield, ?Default\]](#BindingIdentifier) [CallSignature](#CallSignature) ___{___ [FunctionBody\[Yield\]](#FunctionBody) ___}___

### <a name="GeneratorExpression"> GeneratorExpression
- ___function___ ___*___ [BindingIdentifier\[Yield\]\(opt\)](#BindingIdentifier) [CallSignature](#CallSignature) ___{___ [FunctionBody\[Yield\]](#FunctionBody) ___}___

### <a name="YieldExpression"> YieldExpression[In]
- ___yield___ 
- ___yield___ \[no LineTerminator here\] \[Lexical goal InputElementRegExp\] [AssignmentExpression\[?In, Yield\]](./EXPRESSION_SYNTAX_GRAMMAR.md#AssignmentExpression)
- ___yield___ \[no LineTerminator here\] ___*___ \[Lexical goal InputElementRegExp\] [AssignmentExpression\[?In, Yield\]](./EXPRESSION_SYNTAX_GRAMMAR.md#AssignmentExpression)

### <a name="PropertySetParameterList"> PropertySetParameterList
- [FormalParameter](#FormalParameter) [TypeAnnotation\(opt\)](./TYPE_SYNTAX_GRAMMAR.md#TypeAnnotation)

### <a name="ClassDeclaration"> ClassDeclaration\[Yield, Default\]
- ___class___ [BindingIdentifier\[?Yield, ?Default\]](#BindingIdentifier) [ClassTail\[?Yield\]](#ClassTail)

### <a name="ClassExpression"> ClassExpression\[Yield,GeneratorParameter\]
- ___class___ [BindingIdentifier\[?Yield\]\(opt\)](#BindingIdentifier) [ClassTail\[?Yield,?GeneratorParameter\]](#ClassTail)

### <a name="ClassTail"> ClassTail\[Yield,GeneratorParameter\] :
- \[~GeneratorParameter\] [ClassHeritage\[?Yield\]\(opt\)](#ClassHeritage) ___{___ [ClassBody\[?Yield\]\(opt\)](#ClassBody) ___}___
- \[+GeneratorParameter\] [ClassHeritage\(opt\)](#ClassHeritageopt) ___{___ [ClassBodyopt](#ClassBodyopt) ___}___

### <a name="ClassHeritage"> ClassHeritage
- ___extends___ [ClassType](#ClassType)

### <a name="ClassType"> ClassType
- [TypeReference](#TypeReference)

### <a name="ClassBody"> ClassBody\[Yield\]
- [ClassElementList\[?Yield\]](#ClassElementList)

### <a name="ClassElementList"> ClassElementList\[Yield\]
- [ClassElement\[?Yield\]](#ClassElement)
- [ClassElementList\[?Yield\]](#ClassElementList) [ClassElement\[?Yield\]](#ClassElement)

### <a name="ClassElement"> ClassElement\[Yield\]
- [ConstructorDeclaration](#ConstructorDeclaration)
- [PropertyMemberDeclaration](#PropertyMemberDeclaration)
- [IndexMemberDeclaration](#IndexMemberDeclaration)

### <a name="ConstructorDeclaration"> ConstructorDeclaration
- [ConstructorOverloadsopt](#ConstructorOverloadsopt) [ConstructorImplementation](#ConstructorImplementation)

### <a name="ConstructorOverloads"> ConstructorOverloads
- [ConstructorOverload](#ConstructorOverload)
- [ConstructorOverloads](#ConstructorOverloads) [ConstructorOverload](#ConstructorOverload)

### <a name="ConstructorOverload"> ConstructorOverload
- [PublicOrPrivateopt](#PublicOrPrivateopt) ___constructor___ ___(___ [FormalParameters\(opt\)](#FormalParameters) ___)___ ___;___

### <a name="ConstructorImplementation"> ConstructorImplementation
- [PublicOrPrivateopt](#PublicOrPrivateopt) ___constructor___ ___(___ [FormalParameters\(opt\)](#FormalParameters) ___)___ ___{___ [FunctionBody](#FunctionBody) ___}___

### <a name="PropertyMemberDeclaration"> PropertyMemberDeclaration
- [MemberVariableDeclaration](#MemberVariableDeclaration)
- [MemberFunctionDeclaration](#MemberFunctionDeclaration)
- [MemberAccessorDeclaration](#MemberAccessorDeclaration)
- [GeneratorMethodDeclaration\[?Yield\]](#GeneratorMethodDeclaration)

### <a name="MemberVariableDeclaration"> MemberVariableDeclaration
- [PublicOrPrivateopt](#PublicOrPrivateopt) ___static\(opt\)___ [PropertyName](#PropertyName) [TypeAnnotation\(opt\)](#TypeAnnotation) [Initialiser\(opt\)](#Initialiser) ___;___

### <a name="MemberFunctionDeclaration"> MemberFunctionDeclaration
- [MemberFunctionOverloads\(opt\)](#MemberFunctionOverloads) [MemberFunctionImplementation](#MemberFunctionImplementation)

### <a name="MemberFunctionOverloads"> MemberFunctionOverloads
- [MemberFunctionOverload](#MemberFunctionOverload)
- [MemberFunctionOverloads](#MemberFunctionOverloads) [MemberFunctionOverload](#MemberFunctionOverload)

### <a name="MemberFunctionOverload"> MemberFunctionOverload
- [PublicOrPrivateopt](#PublicOrPrivateopt) ___static\(opt\)___ [PropertyName](#PropertyName) [CallSignature](./TYPE_SYNTAX_GRAMMAR.md#CallSignature) ___;___

### <a name="MemberFunctionImplementation"> MemberFunctionImplementation
- [PublicOrPrivateopt](#PublicOrPrivateopt) ___static\(opt\)___ [PropertyName](#PropertyName) [CallSignature](./TYPE_SYNTAX_GRAMMAR.md#CallSignature) ___{___ [FunctionBody](#FunctionBody) ___}___

### <a name="MemberAccessorDeclaration"> MemberAccessorDeclaration
- [PublicOrPrivateopt](#PublicOrPrivateopt) ___static\(opt\)___ [GetAccessor](#GetAccessor)
- [PublicOrPrivateopt](#PublicOrPrivateopt) ___static\(opt\)___ [SetAccessor](#SetAccessor)

### <a name="IndexMemberDeclaration"> IndexMemberDeclaration
- [IndexSignature](#IndexSignature) ___;___

### <a name="EnumDeclaration"> EnumDeclaration
- ___enum___ [Identifier](#Identifier) ___{___ [EnumBody\(opt\)](#EnumBody) ___}___

### <a name="EnumBody"> EnumBody
- [ConstantEnumMembers](#ConstantEnumMembers) ___,___\(opt\)
- [ConstantEnumMembers](#ConstantEnumMembers) ___,___ [EnumMemberSections](#EnumMemberSections) ___,\(opt\)___ [EnumMemberSections](#EnumMemberSections) ___,\(opt\)___

### <a name="ConstantEnumMembers"> ConstantEnumMembers
- [PropertyName](#PropertyName)
- [ConstantEnumMembers](#ConstantEnumMembers) ___,___ [PropertyName](#PropertyName)

### <a name="EnumMemberSections"> EnumMemberSections
- [EnumMemberSection](#EnumMemberSection)
- [EnumMemberSections](#EnumMemberSections) ___,___ [EnumMemberSection](#EnumMemberSection)

### <a name="EnumMemberSection"> EnumMemberSection
- [ConstantEnumMemberSection](#ConstantEnumMemberSection) [ComputedEnumMember](#ComputedEnumMember)

### <a name="ConstantEnumMemberSection"> ConstantEnumMemberSection
- [PropertyName](#PropertyName) ___=___ [NumericLiteral](#NumericLiteral)
- [PropertyName](#PropertyName) ___=___ [NumericLiteral](#NumericLiteral) ___,___ [ConstantEnumMembers](#ConstantEnumMembers)

### <a name="ComputedEnumMember"> ComputedEnumMember
- [PropertyName](#PropertyName) ___=___ [AssignmentExpression](#AssignmentExpression)

### <a name="InterfaceDeclaration"> InterfaceDeclaration
- ___interface___ [Identifier](./EXPRESSION_SYNTAX_GRAMMAR.md#Identifier) [TypeParameters\(opt\)](./TYPE_SYNTAX_GRAMMAR.md#TypeParameters) [InterfaceExtendsClause\(opt\)](#InterfaceExtendsClause) [ObjectType](./TYPE_SYNTAX_GRAMMAR.md#ObjectType)

### <a name="InterfaceExtendsClause"> InterfaceExtendsClause
- ___extends___ [ClassOrInterfaceTypeList](#ClassOrInterfaceTypeList)

### <a name="ClassOrInterfaceTypeList"> ClassOrInterfaceTypeList
- [ClassOrInterfaceType](#ClassOrInterfaceType)
- [ClassOrInterfaceTypeList](#ClassOrInterfaceTypeList) ___,___ [ClassOrInterfaceType](#ClassOrInterfaceType)

### <a name="ClassOrInterfaceType"> ClassOrInterfaceType
- [TypeReference](./TYPE_SYNTAX_GRAMMAR.md#TypeReference)

