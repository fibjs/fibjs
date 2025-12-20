/**
 * TypeScript Type Erasure Test Cases
 * 
 * This file tests the util.stripTypeScript function to understand
 * the expected behavior of TypeScript type stripping.
 * 
 * Run with fibjs:  fibjs ts_strip_test.js
 * Run with amaro:  USE_AMARO=1 node --test ts_strip_test.js
 */

const { describe, it } = require('node:test');
const assert = require('assert');

// Check if we should use amaro (Node.js) or util.stripTypeScript (fibjs)
const USE_AMARO = typeof process !== 'undefined' && process.env.USE_AMARO === '1';

let stripImpl;
let amaroStrip = null;
let fibjsStrip = null;

if (USE_AMARO) {
    const amaro = require('amaro');
    stripImpl = (code) => amaro.transformSync(code, { mode: 'strip-only' }).code;
    amaroStrip = stripImpl;
    console.log('Using amaro for TypeScript stripping');
} else {
    const util = require('util');
    stripImpl = util.stripTypeScript;
    fibjsStrip = stripImpl;
    console.log('Using util.stripTypeScript for TypeScript stripping');
}

/**
 * Helper function to strip TypeScript types
 * @param {string} code - TypeScript code
 * @returns {string} - JavaScript code with types removed
 */
function strip(code) {
    return stripImpl(code);
}

/**
 * Helper to normalize whitespace for comparison
 * @param {string} code 
 * @returns {string}
 */
function normalize(code) {
    return code.replace(/\s+/g, ' ').trim();
}

/**
 * Test both implementations separately for comparison
 * In fibjs mode: runs fibjs test
 * In amaro mode: runs amaro test  
 * This allows reviewing differences between implementations
 * @param {string} name - Test name
 * @param {object} opts - Test options
 * @param {string} opts.input - Input TypeScript code
 * @param {string|Function} [opts.fibjs] - Expected output for fibjs, or test function
 * @param {string|Function} [opts.amaro] - Expected output for amaro, or test function
 * @param {string} [opts.expectedBoth] - Expected output if both should match
 * @param {boolean} [opts.amaroThrows] - Whether amaro should throw
 * @param {boolean} [opts.fibjsThrows] - Whether fibjs should throw
 */
function itBothImpl(name, opts) {
    const { input, fibjs: fibjsExpected, amaro: amaroExpected, expectedBoth, amaroThrows, fibjsThrows } = opts;
    
    if (USE_AMARO) {
        // Running in amaro mode
        it(`${name} [amaro]`, () => {
            if (amaroThrows) {
                assert.throws(() => strip(input), amaroThrows === true ? undefined : amaroThrows);
                return;
            }
            const output = strip(input);
            const expected = expectedBoth || amaroExpected;
            if (typeof expected === 'function') {
                expected(output, input);
            } else if (expected !== undefined) {
                assert.strictEqual(output, expected);
            }
        });
    } else {
        // Running in fibjs mode
        it(`${name} [fibjs]`, () => {
            if (fibjsThrows) {
                assert.throws(() => strip(input), fibjsThrows === true ? undefined : fibjsThrows);
                return;
            }
            const output = strip(input);
            const expected = expectedBoth || fibjsExpected;
            if (typeof expected === 'function') {
                expected(output, input);
            } else if (expected !== undefined) {
                assert.strictEqual(output, expected);
            }
        });
    }
}

/**
 * Test with different expected values for fibjs and amaro
 * @param {string} name - Test name
 * @param {string} input - Input TypeScript code
 * @param {string} expectedFibjs - Expected output for fibjs
 * @param {string} expectedAmaro - Expected output for amaro (use null if amaro throws)
 */
function itDiff(name, input, expectedFibjs, expectedAmaro) {
    it(name, () => {
        if (USE_AMARO && expectedAmaro === null) {
            // amaro throws for this input
            assert.throws(() => strip(input));
        } else {
            const expected = USE_AMARO ? expectedAmaro : expectedFibjs;
            assert.strictEqual(strip(input), expected);
        }
    });
}

/**
 * Test that throws with different error handling for fibjs and amaro
 * @param {string} name - Test name
 * @param {string} input - Input TypeScript code
 * @param {RegExp} fibjsErrorPattern - Error pattern for fibjs (use null if no throw)
 * @param {RegExp|null} amaroErrorPattern - Error pattern for amaro (use null if no throw)
 */
function itThrowsDiff(name, input, fibjsErrorPattern, amaroErrorPattern) {
    it(name, () => {
        if (USE_AMARO) {
            if (amaroErrorPattern === null) {
                strip(input); // should not throw
            } else {
                let threw = false;
                try {
                    strip(input);
                } catch (e) {
                    threw = true;
                    // amaro throws objects with .message, not Error instances
                    const message = e && (e.message || String(e));
                    assert.ok(amaroErrorPattern.test(message), 
                        `Expected error message to match ${amaroErrorPattern}, got: ${message}`);
                }
                assert.ok(threw, `Expected strip() to throw for input: ${input}`);
            }
        } else {
            if (fibjsErrorPattern === null) {
                strip(input); // should not throw
            } else {
                assert.throws(() => strip(input), fibjsErrorPattern);
            }
        }
    });
}

describe('TypeScript Type Erasure Tests', () => {

    describe('Basic Type Annotations', () => {

        it('should strip variable type annotation', () => {
            assert.strictEqual(strip('const x: string = "hello";'), 'const x         = "hello";');
        });

        it('should strip number type annotation', () => {
            assert.strictEqual(strip('let count: number = 42;'), 'let count         = 42;');
        });

        it('should strip boolean type annotation', () => {
            assert.strictEqual(strip('var flag: boolean = true;'), 'var flag          = true;');
        });

        it('should strip union type annotation', () => {
            assert.strictEqual(strip('const value: string | number = "test";'), 'const value                  = "test";');
        });

        it('should strip array type annotation', () => {
            assert.strictEqual(strip('const arr: number[] = [1, 2, 3];'), 'const arr           = [1, 2, 3];');
        });

        it('should strip generic array type annotation', () => {
            assert.strictEqual(strip('const arr: Array<string> = ["a", "b"];'), 'const arr                = ["a", "b"];');
        });

        it('should strip qualified name type annotation', () => {
            assert.strictEqual(strip('const x: A.B = 1;'), 'const x      = 1;');
        });

        it('should strip qualified name type in block', () => {
            assert.strictEqual(strip('{ const x: A.B = 1; }'), '{ const x      = 1; }');
        });

        it('should strip qualified name array type in block', () => {
            assert.strictEqual(strip('{ const x: A.B[] = []; }'), '{ const x        = []; }');
        });

        it('should strip qualified name type in function body', () => {
            assert.strictEqual(strip('function f() { const x: ts.Node = null; }'), 'function f() { const x          = null; }');
        });

        it('should strip constructor type annotation', () => {
            assert.strictEqual(strip('let x: new () => T;'), 'let x             ;');
        });

        it('should strip constructor type with parameters', () => {
            assert.strictEqual(strip('let x: new (a: number) => T;'), 'let x                      ;');
        });

        it('should strip function type annotation', () => {
            assert.strictEqual(strip('let x: () => T;'), 'let x         ;');
        });

        it('should strip function type with parameters', () => {
            assert.strictEqual(strip('let x: (a: number) => T;'), 'let x                  ;');
        });

    });

    describe('Function Type Annotations', () => {

        it('should strip function parameter types', () => {
            assert.strictEqual(strip('function greet(name: string) { return name; }'), 'function greet(name        ) { return name; }');
        });

        it('should strip function return type', () => {
            assert.strictEqual(strip('function add(a: number, b: number): number { return a + b; }'), 'function add(a        , b        )         { return a + b; }');
        });

        it('should strip type predicate return type', () => {
            assert.strictEqual(strip('function isString(x: any): x is string { return typeof x === "string"; }'), 'function isString(x     )              { return typeof x === "string"; }');
        });

        it('should strip type predicate with generic type', () => {
            assert.strictEqual(strip('function isArray(x): x is Array<any> { return true; }'), 'function isArray(x)                  { return true; }');
        });

        it('should strip type predicate in arrow function', () => {
            assert.strictEqual(strip('const isNum = (x: any): x is number => typeof x === "number";'), 'const isNum = (x     )              => typeof x === "number";');
        });

        it('should strip arrow function types', () => {
            assert.strictEqual(strip('const fn = (x: number): string => x.toString();'), 'const fn = (x        )         => x.toString();');
        });

        it('should strip optional parameter', () => {
            assert.strictEqual(strip('function test(a: string, b?: number) { return a; }'), 'function test(a        , b         ) { return a; }');
        });

        it('should strip optional parameter with contextual keyword name', () => {
            // 'type' is a contextual keyword but can be used as parameter name
            assert.strictEqual(strip('function f(type?: string) {}'), 'function f(type         ) {}');
        });

        it('should strip multiple optional parameters with keyword names', () => {
            assert.strictEqual(strip('function f(get?: number, set?: string) {}'), 'function f(get         , set         ) {}');
        });

        it('should strip default parameter with type', () => {
            assert.strictEqual(strip('function test(x: number = 10) { return x; }'), 'function test(x         = 10) { return x; }');
        });

        it('should strip parameter types inside arrow function body block', () => {
            const input = '(() => { function test(description: string, source: string, verifier: (file: ts.SourceFile, checker: ts.TypeChecker, program: ts.Program) => void) { return source; } })();';
            const out = strip(input);
            assert.strictEqual(
                normalize(out),
                '(() => { function test(description , source , verifier ) { return source; } })();'
            );
        });

    });

    describe('Generic Types', () => {

        it('should strip generic function declaration', () => {
            assert.strictEqual(strip('function identity<T>(arg: T): T { return arg; }'), 'function identity   (arg   )    { return arg; }');
        });

        it('should strip multiple generic parameters', () => {
            assert.strictEqual(strip('function pair<K, V>(key: K, value: V): [K, V] { return [key, value]; }'), 'function pair      (key   , value   )         { return [key, value]; }');
        });

        it('should strip generic constraint', () => {
            assert.strictEqual(strip('function longest<T extends { length: number }>(a: T, b: T): T { return a.length > b.length ? a : b; }'), 'function longest                              (a   , b   )    { return a.length > b.length ? a : b; }');
        });

        it('should strip generic class', () => {
            assert.strictEqual(strip('class Box<T> { value: T; }'), 'class Box    { value   ; }');
        });

    });

    describe('Interface and Type Alias', () => {

        it('should remove interface declaration completely', () => {
            assert.strictEqual(strip('interface User { name: string; age: number; }'), '                                             ');
        });

        it('should remove type alias completely', () => {
            assert.strictEqual(strip('type Status = "active" | "inactive";'), '                                    ');
        });

        it('should remove complex type alias', () => {
            assert.strictEqual(strip('type Callback<T> = (data: T) => void;'), '                                     ');
        });

    });

    describe('Type Assertions', () => {

        it('should strip as assertion', () => {
            assert.strictEqual(strip('const x = value as string;'), 'const x = value          ;');
        });

        it('should strip as assertion inside parenthesized comparison operand', () => {
            const input = 'for (let index = 0; index < (value as unknown[]).length; index++) {}';
            const out = strip(input);
            assert.strictEqual(normalize(out), 'for (let index = 0; index < (value ).length; index++) {}');
        });

        it('should strip as assertion in parens', () => {
            assert.strictEqual(strip('const x = (value as string);'), 'const x = (value          );');
        });

        it('should strip non-null assertion', () => {
            assert.strictEqual(strip('const x = value!;'), 'const x = value ;');
        });

        it('should strip satisfies expression', () => {
            assert.strictEqual(strip('const x = { name: "test" } satisfies User;'), 'const x = { name: "test" }               ;');
        });

    });

    describe('Class Members', () => {

        it('should strip class property type', () => {
            assert.strictEqual(strip('class Test { name: string = ""; }'), 'class Test { name         = ""; }');
        });

        it('should strip access modifiers with types', () => {
            assert.strictEqual(strip('class Test { private name: string = ""; public age: number = 0; }'), 'class Test {         name         = "";        age         = 0; }');
        });

        it('should strip readonly modifier with type', () => {
            assert.strictEqual(strip('class Test { readonly id: number = 1; }'), 'class Test {          id         = 1; }');
        });

        // amaro doesn't support constructor parameter properties in strip-only mode
        itThrowsDiff('should strip constructor parameter properties',
            'class Test { constructor(public name: string, private age: number) {} }',
            null, // fibjs: succeeds
            /parameter property.*not supported/); // amaro: throws

        it('should strip class method with three parameters', () => {
            // Bug: second parameter type was not stripped
            const input = 'class A { parse(a: string, b: HTMLElement, c: HTMLElement) {} }';
            const out = strip(input);
            assert.ok(!out.includes(':'), out);
            assert.doesNotThrow(() => new Function(out), out);
        });

        it('should strip class method with protected async and three parameters', () => {
            const input = 'class A { protected async parse(a: string, b: HTMLElement, c: HTMLElement): Promise<void> {} }';
            const out = strip(input);
            assert.ok(!out.includes(':'), out);
            assert.doesNotThrow(() => new Function(out), out);
        });

        it('should strip abstract class members', () => {
            const input = 'abstract class Animal { abstract speak(): void; }';
            const out = strip(input);
            assert.ok(out.includes('class Animal'), out);
            assert.ok(!out.includes('abstract'), out);
            assert.ok(!out.includes('speak()'), out);
            assert.doesNotThrow(() => new Function(out), out);
        });

        it('should strip abstract keyword in class declaration', () => {
            const input = 'abstract class Animal { speak(): void { return; } }';
            const out = strip(input);
            assert.ok(!out.includes('abstract class'), out);
            assert.ok(out.includes('class Animal'), out);
        });

        it('should strip implements clause', () => {
            assert.strictEqual(strip('class Dog implements Animal { speak() {} }'), 'class Dog                   { speak() {} }');
        });

        it('should strip implements clause with generic type parameters', () => {
            assert.strictEqual(strip('class testClass8 implements IFoo<{ x: V }> { }'), 'class testClass8                           { }');
        });

        it('should strip implements clause with multiple interfaces', () => {
            assert.strictEqual(strip('class Test implements Foo, Bar<T> {}'), 'class Test                        {}');
        });

        it('should strip constructor overload signature without body', () => {
            const input = `class A {
    constructor(x: string);
    constructor(x: string) { this.x = x; }
}`;
            const out = strip(input);
            // No constructor signature should remain.
            assert.ok(!/constructor\([^)]*\)\s*;/.test(out), out);
            assert.doesNotThrow(() => new Function(out), out);
        });

        it('should strip class property type with decorated export class', () => {
            const input = `@deco
export class ClassA {
    array: SomeClass[];
}`;
            const out = strip(input);
            assert.ok(!out.includes('SomeClass'), 'should not contain SomeClass type');
            assert.ok(out.includes('array'), 'should contain property name');
        });

    });

    describe('Import/Export Types', () => {

        it('should remove type-only import entirely', () => {
            // import type { ... } is a type-only import, should be entirely removed
            const input = 'import type { User } from "./types";';
            const expected = '                                    ';
            assert.strictEqual(strip(input), expected);
        });

        it('should remove multiline type-only import entirely', () => {
            const input = `import type {
    A,
    B,
    C
} from "./x";`;
            const result = strip(input);
            assert.ok(!result.includes('A'), 'should not contain A');
            assert.ok(!result.includes('B'), 'should not contain B');
            assert.ok(!result.includes('C'), 'should not contain C');
            assert.ok(!result.includes('from'), 'should not contain from');
        });

        it('should remove type-only namespace import', () => {
            const input = 'import type * as Types from "./types";';
            const expected = '                                      ';
            assert.strictEqual(strip(input), expected);
        });

        it('should remove type-only export entirely', () => {
            // export type { ... } is a type-only export, should be entirely removed
            const input = 'export type { User };';
            const expected = '                     ';
            assert.strictEqual(strip(input), expected);
        });

        it('should remove type-only re-export entirely', () => {
            // export type { ... } from '...' is a type-only re-export, should be entirely removed
            const input = "export type { A, B, C } from './types';";
            const expected = '                                       ';
            assert.strictEqual(strip(input), expected);
        });

        it('should preserve export as namespace (UMD declaration)', () => {
            // export as namespace X is a UMD global namespace declaration
            // It's a runtime declaration, not type-only, so preserve it
            const input = 'export as namespace MyLib;';
            const expected = 'export as namespace MyLib;';
            assert.strictEqual(strip(input), expected);
        });

        // Both fibjs and amaro completely remove type-only imports including the identifier
        it('should handle inline type import', () => {
            assert.strictEqual(strip('import { type User, getData } from "./module";'), 'import {            getData } from "./module";');
        });

        it('should remove export type alias with arrow function type', () => {
            // Bug: export type X = (T) => R was leaving "=> R" behind
            const input = 'export type Fn = (x: T) => R;';
            const out = strip(input);
            assert.ok(!out.includes('=>'), out);
            assert.doesNotThrow(() => new Function(out), out);
        });

        it('should remove export type alias with generic arrow function type', () => {
            const input = 'export type GetResult<T extends Base, R extends Result> = (resolution: T) => R | undefined;';
            const out = strip(input);
            assert.ok(!out.includes('=>'), out);
            assert.doesNotThrow(() => new Function(out), out);
        });

        it('should remove import type with dotted access', () => {
            // import("module").Foo.Bar should be fully stripped
            const input = 'let x: import("mocha").reporters.XUnit | undefined;';
            const expected = 'let x                                             ;';
            assert.strictEqual(strip(input), expected);
        });

        it('should preserve JSX-style const assertion <const>', () => {
            // <const> is a const assertion that should be preserved, not a type assertion to erase
            const input = 'let q1 = <const> 10;';
            const expected = 'let q1 = <const> 10;';
            assert.strictEqual(strip(input), expected);
        });

        it('should preserve various forms of const assertions', () => {
            const input = `let q1 = <const> 10;
let q2 = <const> 'abc';
let q3 = <const> true;
let q4 = <const> [1, 2, 3];`;
            const out = strip(input);
            assert.ok(out.includes('<const>'), 'Should preserve <const> assertions');
            // Each line should still have <const>
            const lines = out.split('\n');
            assert.strictEqual(lines.length, 4);
            lines.forEach(line => {
                assert.ok(line.includes('<const>'), `Line should contain <const>: ${line}`);
            });
        });

        it('should remove type assertion but keep const assertion', () => {
            const input = `let v1 = <string>'abc';
let v2 = <const> 'abc';`;
            const out = strip(input);
            assert.ok(!out.includes('<string>'), 'Should remove <string> type assertion');
            assert.ok(out.includes('<const>'), 'Should preserve <const> assertion');
        });

        it('should remove import type with deeply nested dotted access', () => {
            const input = 'let x: import("foo").a.b.c.d;';
            const expected = 'let x                       ;';
            assert.strictEqual(strip(input), expected);
        });

        it('should remove decorator and this parameter from function', () => {
            // Decorator on this parameter should be removed along with the this parameter
            const input = 'function direct(@dec this: C) { return this.n; }';
            const expected = 'function direct(            ) { return this.n; }';
            assert.strictEqual(strip(input), expected);
        });

        it('should remove this parameter with type from function', () => {
            const input = 'function foo(this: C, x: number) { return x; }';
            const expected = 'function foo(         x        ) { return x; }';
            assert.strictEqual(strip(input), expected);
        });

        it('should preserve parameter decorators', () => {
            // Parameter decorators are preserved (matches amaro behavior)
            const input = 'function foo(@dec x: number) { return x; }';
            const out = strip(input);
            assert.ok(out.includes('@dec'), 'Should preserve parameter decorator');
            assert.ok(!out.includes(': number'), 'Should remove type annotation');
        });

        it('should handle static as property name', () => {
            // Second 'static' is a property name, not a modifier
            const input = 'class C { static static m() {} }';
            const expected = 'class C { static static m() {} }';
            assert.strictEqual(strip(input), expected);
        });

        it('should handle static static with type annotation', () => {
            const input = 'class C { static static [x: string]: string; }';
            const out = strip(input);
            assert.ok(out.includes('static static'), 'Should preserve static static');
            assert.ok(!out.includes('[x: string]'), 'Should remove type annotation');
        });

        it('should remove decorator from this parameter in class method', () => {
            const input = 'class C { method(@dec this: C) {} }';
            const expected = 'class C { method(            ) {} }';
            assert.strictEqual(strip(input), expected);
        });

        it('should remove decorator from this parameter with other parameters', () => {
            // When 'this' is not the first parameter, it's treated as a regular identifier
            const input = 'class C { method(@dec allowed: C, @dec this: C) {} }';
            const out = strip(input);
            assert.ok(out.includes('@dec allowed'), 'Should preserve decorator on regular param');
            assert.ok(out.includes('@dec this'), 'Should preserve this as regular param name in non-first position');
            assert.ok(!out.includes(': C'), 'Should remove type annotations');
        });

        it('should handle static override static', () => {
            const input = 'class C { static override static }';
            const out = strip(input);
            assert.ok(out.includes('static'), 'Should preserve static keyword');
            assert.ok(!out.includes('override'), 'Should remove override modifier');
        });

        it('should remove abstract on same line', () => {
            const input = 'abstract class A {}';
            const out = strip(input);
            assert.ok(!out.includes('abstract'), 'Should remove abstract modifier');
            assert.ok(out.includes('class A'), 'Should keep class');
        });

        it('should preserve abstract with line break (ASI)', () => {
            const input = 'abstract\nclass B {}';
            const out = strip(input);
            assert.ok(out.includes('abstract'), 'Should preserve abstract as identifier due to ASI');
            assert.ok(out.includes('class B'), 'Should keep class');
        });

        it('should handle type alias with double less-than', () => {
            // ReturnType<<T> where << is tokenized as LessThanLessThanToken
            const input = 'type Bar = ReturnType<<T>(x: T) => number>;';
            const out = strip(input);
            assert.ok(!out.includes('type'), 'Should remove type alias');
            assert.ok(!out.includes('ReturnType'), 'Should remove everything');
            assert.ok(!out.includes('<<T>'), 'Should not leave << behind');
        });

        it('should handle call expression with double less-than type args', () => {
            // foo<<T>(x: T) => number>() where << is tokenized as LessThanLessThanToken
            const input = 'function foo<T>(_x: T) {}\nconst b = foo<<T>(x: T) => number>(() => 1);';
            const out = strip(input);
            assert.ok(out.includes('function foo'), 'Should keep function');
            assert.ok(out.includes('const b = foo'), 'Should keep const');
            assert.ok(!out.includes('<T>'), 'Should remove all type arguments');
            assert.ok(!out.includes('<<T>'), 'Should remove double less-than type arguments');
            assert.ok(out.includes('_x'), 'Should keep parameter name');
            assert.ok(out.includes('() => 1'), 'Should keep callback');
        });

        it('should remove type annotations in parameter decorator arguments', () => {
            // Arrow function with type annotation inside parameter decorator
            const input = 'class C { m(@dec((x: T) => x) p) {} }';
            const out = strip(input);
            assert.ok(out.includes('class C'), 'Should keep class');
            assert.ok(out.includes('@dec'), 'Should keep decorator');
            assert.ok(!out.includes(': T'), 'Should remove type annotation');
            assert.ok(out.includes('(x   ) => x'), 'Should keep arrow function with type removed');
        });

        it('should remove instantiation expression before optional chain', () => {
            // a<b>?.() where <b> is an instantiation expression
            const input = 'a<b>?.();';
            const out = strip(input);
            assert.ok(!out.includes('<b>'), 'Should remove type arguments');
            assert.ok(out.includes('a'), 'Should keep identifier');
            assert.ok(out.includes('?.()'), 'Should keep optional chain call');
        });

        it('should remove interface with complex extends clause', () => {
            // interface B extends A<{}, { x: {} }> {}
            const input = 'interface B extends A<{}, { x: {} }> {}';
            const out = strip(input);
            assert.ok(!out.includes('interface'), 'Should remove interface');
            assert.ok(!out.includes('extends'), 'Should remove extends');
            assert.ok(!out.includes('{'), 'Should remove everything');
        });

        it('should handle duplicate modifier keywords as property names', () => {
            // protected protected: any - first is modifier, second is property name
            const input = 'class A { protected protected: any; }';
            const out = strip(input);
            assert.ok(out.includes('protected'), 'Should keep second protected as property name');
            assert.ok(!out.includes('any'), 'Should remove type annotation');
            const protectedCount = (out.match(/protected/g) || []).length;
            assert.strictEqual(protectedCount, 1, 'Should have exactly one protected (as property name)');
        });

        it('should remove export type with assert clause', () => {
            // export type {} from './0' assert { type: "json" }
            const input = 'export type {} from "./0" assert { type: "json" }';
            const out = strip(input);
            assert.ok(!out.includes('export'), 'Should remove export');
            assert.ok(!out.includes('assert'), 'Should remove assert clause');
            assert.ok(!out.includes('type'), 'Should remove type keyword');
        });

        it('should remove import type with assert clause', () => {
            // import type { I } from './0' assert { type: "json" }
            const input = 'import type { I } from "./0" assert { type: "json" }';
            const out = strip(input);
            assert.ok(!out.includes('import'), 'Should remove import');
            assert.ok(!out.includes('assert'), 'Should remove assert clause');
            assert.ok(!out.includes('type'), 'Should remove type keyword');
        });

        it('should remove abstract modifier in type', () => {
            // type Foo = abstract new(...args: any) => any
            const input = 'type Foo = abstract new(...args: any) => any;';
            const out = strip(input);
            assert.ok(!out.includes('abstract'), 'Should remove abstract modifier');
            assert.ok(!out.includes('new'), 'Should remove constructor signature');
            assert.ok(!out.includes('Foo'), 'Should remove type alias');
        });

        it('should remove typeof this.member type', () => {
            const input = 'class Foo { foo: number; bar: typeof this.foo }';
            const expected = 'class Foo { foo        ; bar                  }';
            assert.strictEqual(strip(input), expected);
        });

        it('should remove typeof with nested member access', () => {
            const input = 'let x: typeof this.foo.bar.baz;';
            const expected = 'let x                         ;';
            assert.strictEqual(strip(input), expected);
        });

    });

    describe('Declare and Ambient', () => {

        it('should remove declare variable', () => {
            assert.strictEqual(strip('declare const VERSION: string;'), '                              ');
        });

        it('should remove declare function', () => {
            assert.strictEqual(strip('declare function alert(message: string): void;'), '                                              ');
        });

        it('should remove declare module', () => {
            assert.strictEqual(strip('declare module "lodash" { export function get(obj: any, path: string): any; }'), '                                                                             ');
        });

        it('should remove declare module without body', () => {
            const input = 'declare module "foo";\n\nexport default Array;';
            const expected = '                     \n\nexport default Array;';
            assert.strictEqual(strip(input), expected);
        });

        it('should remove declare module without body at file start', () => {
            const input = 'declare module "path";\nimport path from "path";';
            const expected = '                      \nimport path from "path";';
            assert.strictEqual(strip(input), expected);
        });

        it('should remove declare namespace with dotted name', () => {
            const input = 'declare namespace Foo.Bar { export var foo; };\nFoo.Bar.foo = 5;';
            const expected = '                                             ;\nFoo.Bar.foo = 5;';
            assert.strictEqual(strip(input), expected);
        });

        it('should remove declare namespace with deeply nested dotted name', () => {
            const input = 'declare namespace A.B.C.D { }';
            const expected = '                             ';
            assert.strictEqual(strip(input), expected);
        });

        it('should preserve declare as identifier', () => {
            // 'declare' can be a variable name
            const input = 'var declare; declare instanceof C;';
            const expected = 'var declare; declare instanceof C;';
            assert.strictEqual(strip(input), expected);
        });

        it('should remove declare class field with computed property', () => {
            const input = `export class IterableWeakMap {
    declare readonly [Symbol.toStringTag]: "IterableWeakMap";

    #weakMap = new WeakMap();
}`;
            const expected = `export class IterableWeakMap {
                                                             

    #weakMap = new WeakMap();
}`;
            assert.strictEqual(strip(input), expected);
        });

        it('should remove declare field without extra semicolon', () => {
            const input = 'class A { declare foo: string; bar = 1; }';
            const expected = 'class A {                      bar = 1; }';
            assert.strictEqual(strip(input), expected);
        });

        it('should remove abstract method without extra semicolon', () => {
            const input = 'abstract class A { abstract foo(): void; bar() {} }';
            const expected = '         class A {                       bar() {} }';
            assert.strictEqual(strip(input), expected);
        });

        it('should preserve private method call in class field initializer', () => {
            const input = 'class C { static s = C.#method(); static #method() { return 42; } }';
            const expected = 'class C { static s = C.#method(); static #method() { return 42; } }';
            assert.strictEqual(strip(input), expected);
        });

    });

    describe('Line and Column Preservation', () => {

        it('should preserve line count', () => {
            const input = `const x: string = "a";
const y: number = 1;
const z: boolean = true;`;
            const output = strip(input);
            assert.strictEqual(input.split('\n').length, output.split('\n').length);
        });

        it('should preserve position with spaces', () => {
            const input = 'const x: string = "hello";';
            const output = strip(input);
            assert.strictEqual(input.indexOf('"hello"'), output.indexOf('"hello"'));
        });

    });

    describe('Complex Cases', () => {

        it('should handle nested generics', () => {
            assert.strictEqual(strip('const data: Map<string, Array<Promise<number>>> = new Map();'), 'const data                                      = new Map();');
        });

        it('should handle conditional types (in type alias)', () => {
            assert.strictEqual(strip('type IsString<T> = T extends string ? true : false;'), '                                                   ');
        });

        it('should handle mapped types (in type alias)', () => {
            assert.strictEqual(strip('type Readonly<T> = { readonly [K in keyof T]: T[K] };'), '                                                     ');
        });

        // Type alias with template literal type should be completely removed
        it('should handle template literal types (in type alias)', () => {
            assert.strictEqual(strip('type EventName = `on${string}`;'), '                               ');
        });

        it('should handle mixed code', () => {
            const input = `
interface Config {
    debug: boolean;
}

function setup(config: Config): void {
    console.log(config);
}

const cfg: Config = { debug: true };
setup(cfg);
`;
            const expected = `
                  
                   
 

function setup(config        )       {
    console.log(config);
}

const cfg         = { debug: true };
setup(cfg);
`;
            assert.strictEqual(strip(input), expected);
        });

    });

    describe('Enum Handling', () => {
        // amaro throws different error format for enum

        itThrowsDiff('should throw for const enum (not supported in strip-only mode)',
            'const enum Color { Red, Green, Blue }',
            /enum.*not supported/, // fibjs error message
            /TypeScript enum.*not supported/); // amaro error message

        itThrowsDiff('should throw for regular enum (not supported in strip-only mode)',
            'enum Direction { Up, Down, Left, Right }',
            /enum.*not supported/, // fibjs error message
            /TypeScript enum.*not supported/); // amaro error message

    });

    describe('Namespace Handling', () => {
        // Both fibjs and amaro throw for namespace in strip-only mode

        itThrowsDiff('should preserve namespace (strip-only mode)',
            'namespace MyLib { export const version = "1.0"; }',
            /namespace.*not supported/, // fibjs: throws
            /namespace.*not supported/); // amaro: throws

    });

    // ========================================
    // Additional test cases discovered from SWC
    // ========================================

    describe('This Parameter', () => {

        it('should strip this parameter in function', () => {
            assert.strictEqual(strip('function foo(this: number) {}'), 'function foo(            ) {}');
        });

        it('should strip this parameter in method', () => {
            assert.strictEqual(strip('const obj = { method(this: MyClass, x: number) { return x; } };'), 'const obj = { method(               x        ) { return x; } };');
        });

        it('should strip this parameter with optional type annotation', () => {
            assert.strictEqual(strip('function foo(this?: MyClass) {}'), 'function foo(              ) {}');
        });

        it('should erase this parameter even without type annotation', () => {
            // TypeScript this parameter as first parameter should always be erased
            // because JavaScript does not allow 'this' as a parameter name
            const input = '({ method(this) {} })';
            const output = strip(input);
            // 'this' should be erased
            assert.strictEqual(output, '({ method(    ) {} })');
        });

        it('should erase this parameter with following parameters', () => {
            const input = 'function f(this, a, b) {}';
            const output = strip(input);
            assert.strictEqual(output, 'function f(      a, b) {}');
        });

    });

    describe('Instantiation Expression', () => {

        // Instantiation expression without semicolon should still strip the generic
        it('should handle instantiation expression without semicolon', () => {
            assert.strictEqual(strip('const foo = call<string>'), 'const foo = call        ');
        });

        it('should strip instantiation expression with semicolon', () => {
            assert.strictEqual(strip('const fn = genericFn<string, number>;'), 'const fn = genericFn                ;');
        });

        it('should strip instantiation expression followed by instanceof', () => {
            assert.strictEqual(strip('Box<number> instanceof Object;'), 'Box         instanceof Object;');
        });

        it('should strip instantiation expression followed by in', () => {
            assert.strictEqual(strip('x = f<number> in obj;'), 'x = f         in obj;');
        });

    });

    describe('As Const Assertion', () => {

        it('should strip as const assertion', () => {
            assert.strictEqual(strip('const foo = [1, 3, 5] as const;'), 'const foo = [1, 3, 5]         ;');
        });

        it('should strip as const on object', () => {
            assert.strictEqual(strip('const obj = { a: 1, b: 2 } as const;'), 'const obj = { a: 1, b: 2 }         ;');
        });

    });

    describe('Satisfies Expression', () => {

        it('should strip satisfies expression', () => {
            assert.strictEqual(strip('const foo = 1 satisfies number;'), 'const foo = 1                 ;');
        });

        it('should strip complex satisfies expression', () => {
            const input = `const foo = {
    foo: 1 as number,
    bar: "bar" as any as number,
} satisfies number;`;
            const expected = `const foo = {
    foo: 1          ,
    bar: "bar"                 ,
}                 ;`;
            assert.strictEqual(strip(input), expected);
        });

    });

    describe('Function Overload', () => {

        it('should strip function overload signatures', () => {
            const input = `function add(a: string, b: string): string;
function add(a: number, b: number): number;
function add(a: string | number, b: string | number): string | number {
    return a as any + b;
}`;
            // Function overload signatures (without body) should be completely erased
            const expected = `                                           
                                           
function add(a                 , b                 )                  {
    return a        + b;
}`;
            assert.strictEqual(strip(input), expected);
        });

        it('should strip method overload signatures in class', () => {
            const input = `class Test {
    foo(bar: string): string;
    foo(bar: number): number;
    foo(bar: string | number): string | number {
        return bar;
    }
}`;
            // Class method overload signatures should be completely erased
            const expected = `class Test {
                             
                             
    foo(bar                 )                  {
        return bar;
    }
}`;
            assert.strictEqual(strip(input), expected);
        });

        it('should strip optional method overload with keyword name in class', () => {
            const input = `class Iterator {
    return?(): void;
    [Symbol.iterator]() { return this; }
}`;
            const out = strip(input);
            assert.ok(!out.includes('return?()'), out);
            assert.strictEqual(normalize(out), normalize(`class Iterator {
    [Symbol.iterator]() { return this; }
}`));
        });

        it('should strip export function overload signatures', () => {
            const input = `export function create(a: string): string;
export function create(a: number): number;
export function create(a: string | number): string | number {
    return a;
}`;
            // Export function overload signatures should be completely erased
            const expected = `                                          
                                          
export function create(a                 )                  {
    return a;
}`;
            assert.strictEqual(strip(input), expected);
        });

        it('should strip function overload signatures without return type', () => {
            const input = `function f(x: string);
function f(x: string) { return x; }`;
            const out = strip(input);
            // No overload signature should remain.
            assert.ok(!/function\s+f\([^)]*\)\s*;/.test(out), out);
            assert.doesNotThrow(() => new Function(out), out);
        });

    });

    describe('Declare Enum', () => {

        it('should remove declare enum', () => {
            assert.strictEqual(strip('declare enum Foo { A, B }'), '                         ');
        });

        it('should remove declare const enum', () => {
            assert.strictEqual(strip('declare const enum Color { Red = 1, Green = 2 }'), '                                               ');
        });

    });

    describe('Non-Instantiated Namespace', () => {
        // Type-only namespace: amaro erases it, fibjs throws
        // This is a complex feature that requires namespace analysis

        itThrowsDiff('should handle type-only namespace',
            `namespace NotInstantiated {
    export interface JustAType { }
    export type ATypeInANamespace = {};
}`,
            /namespace.*not supported/, // fibjs: throws (no namespace analysis)
            null);  // amaro: succeeds (erases type-only namespace)

    });

    describe('Destructuring Parameter Types', () => {

        it('should strip destructuring parameter type annotation', () => {
            assert.strictEqual(strip('function typeAnn({ a, b, c }: { a: number; b: number; c?: number }) {}'), 'function typeAnn({ a, b, c }                                      ) {}');
        });

        it('should strip array destructuring type', () => {
            assert.strictEqual(strip('function test([first, second]: [string, number]) {}'), 'function test([first, second]                  ) {}');
        });

    });

    describe('Async Arrow Function Generic', () => {

        it('should strip async arrow function generic', () => {
            assert.strictEqual(strip('let f = async <T>(v: T) => v;'), 'let f = async    (v   ) => v;');
        });

        it('should strip async arrow with return type', () => {
            assert.strictEqual(strip('let g = async <T>(v: T): Promise<T> => v;'), 'let g = async    (v   )             => v;');
        });

    });

    describe('Override Keyword', () => {

        it('should strip override keyword (TypeScript-only)', () => {
            const input = `class Child extends Parent {
    override method() { return 1; }
}`;
            const expected = `class Child extends Parent {
             method() { return 1; }
}`;
            assert.strictEqual(strip(input), expected);
        });

    });

    describe('Definite Assignment Assertion', () => {

        it('should strip definite assignment assertion on property', () => {
            assert.strictEqual(strip('class Test { name!: string; }'), 'class Test { name         ; }');
        });

        it('should strip definite assignment assertion on variable', () => {
            assert.strictEqual(strip('let x!: number;'), 'let x         ;');
        });

    });

    describe('Comments in Type Positions', () => {

        // fibjs erases all comments; amaro preserves them
        itDiff('should handle comments in optional parameter',
            'function foo(id/** Why? */?: string) {}',
            'function foo(id                    ) {}',  // fibjs: comments erased
            'function foo(id/** Why? */         ) {}'   // amaro: comments preserved
        );

        it('should strip class type parameters when comment follows <', () => {
            const input = 'class C</**doc*/ T> { }';
            const expected = 'class C             { }';
            assert.strictEqual(strip(input), expected);
        });

        it('should strip method type parameters when comment follows <', () => {
            const input = 'class C { method</**doc*/ U extends T>(a: U) { } }';
            const expected = 'class C { method                      (a   ) { } }';
            assert.strictEqual(strip(input), expected);
        });

        it('should strip function type parameters when comment follows <', () => {
            const input = 'function compare</**type*/T>(a: T, b: T) { return a === b; }';
            const expected = 'function compare            (a   , b   ) { return a === b; }';
            assert.strictEqual(strip(input), expected);
        });

    });

    describe('Unicode Identifiers in Types', () => {
        // fibjs uses UTF-8 byte count, amaro uses character count

        it('should handle unicode type alias', () => {
            const input = 'type 任意 = any;';
            const output = strip(input);
            // fibjs: UTF-8 byte count (任意 = 6 bytes), amaro: character count
            const expectedLen = USE_AMARO ? 14 : 18; // 18 bytes for UTF-8
            assert.strictEqual(output.length, expectedLen);
            // Content should be only whitespace
            assert.ok(/^\s*$/.test(output), 'Expected only whitespace');
        });

    });

    describe('Type-only Statements in Control Flow', () => {
        // Type declarations in control flow should be replaced with semicolon to prevent ASI issues

        it('should handle type in if body', () => {
            const input = `if (false) type Foo = string
console.log("Hello");`;
            const expected = `if (false) ;                
console.log("Hello");`;
            assert.strictEqual(strip(input), expected);
        });

        it('should handle interface in while body', () => {
            const input = `while (false)
    interface X { }
console.log("Done");`;
            const expected = `while (false)
    ;              
console.log("Done");`;
            assert.strictEqual(strip(input), expected);
        });

    });

    describe('ASI Edge Cases', () => {
        // Semicolon insertion is needed to prevent ASI issues when type annotation is followed by parenthesis

        it('should handle as expression followed by parenthesis', () => {
            const input = `const x1 = 10 as any
(1)`;
            const expected = `const x1 = 10 ;     
(1)`;
            assert.strictEqual(strip(input), expected);
        });

        it('should handle satisfies followed by operator', () => {
            const input = `const y = 10 satisfies any
    + 1`;
            const expected = `const y = 10              
    + 1`;
            assert.strictEqual(strip(input), expected);
        });

        it('should not insert semicolon when next line has no line break', () => {
            // Same line - no ASI needed
            const input = `const x = 1 as any; (2)`;
            const expected = `const x = 1       ; (2)`;
            assert.strictEqual(strip(input), expected);
        });

        it('should not insert semicolon when previous token is already semicolon', () => {
            const input = `f();
declare const x: number;`;
            const expected = `f();
                        `;
            assert.strictEqual(strip(input), expected);
        });

        it('should handle as expression followed by bracket', () => {
            const input = `const arr = foo as any
[0]`;
            const expected = `const arr = foo ;     
[0]`;
            assert.strictEqual(strip(input), expected);
        });

        it('should handle as expression followed by slash', () => {
            // Slash could be regex start - no semicolon needed for expression level
            const input = `const x = 1 as any
/regex/`;
            const expected = `const x = 1       
/regex/`;
            assert.strictEqual(strip(input), expected);
        });

        it('should handle as expression followed by plus', () => {
            // Plus is safe - continues the expression
            const input = `const x = 1 as number
+1`;
            const expected = `const x = 1          
+1`;
            assert.strictEqual(strip(input), expected);
        });

        it('should handle as expression followed by minus', () => {
            // Minus is safe - continues the expression
            const input = `const x = 1 as number
-1`;
            const expected = `const x = 1          
-1`;
            assert.strictEqual(strip(input), expected);
        });

    });

    describe('Control Flow Statement Body ASI', () => {
        // When a control flow statement body is a TypeScript declaration, insert semicolon

        it('should insert semicolon for while with interface body', () => {
            const input = `while (false) interface X {}`;
            const expected = `while (false) ;             `;
            assert.strictEqual(strip(input), expected);
        });

        it('should insert semicolon for if with type body', () => {
            const input = `if (false) type T = number;`;
            const expected = `if (false) ;               `;
            assert.strictEqual(strip(input), expected);
        });

        it('should insert semicolon for while with type body', () => {
            const input = `while (false) type T = number;`;
            const expected = `while (false) ;               `;
            assert.strictEqual(strip(input), expected);
        });

        it('should insert semicolon for for with interface body', () => {
            const input = `for (;;) interface X {}`;
            const expected = `for (;;) ;             `;
            assert.strictEqual(strip(input), expected);
        });

        it('should insert semicolon for with with type body', () => {
            const input = `with (obj) type T = number;`;
            const expected = `with (obj) ;               `;
            assert.strictEqual(strip(input), expected);
        });

        it('should not insert semicolon for function call before declare', () => {
            // Function call f() should not trigger control flow ASI
            const input = `f()
declare const x: number;`;
            const expected = `f()
                        `;
            assert.strictEqual(strip(input), expected);
        });

        it('should preserve semicolon from previous declare when followed by array', () => {
            // When consecutive declares are removed and followed by [, preserve the ; from first declare
            const input = `foo();
declare const props: any[];
declare const stat: any;
[].push(1);`;
            const expected = `foo();
                          ;
                        
[].push(1);`;
            assert.strictEqual(strip(input), expected);
        });

        it('should not insert semicolon for normal while loop', () => {
            const input = `while (true) { break; }`;
            const expected = `while (true) { break; }`;
            assert.strictEqual(strip(input), expected);
        });

    });

    describe('Class Computed Properties with Modifiers', () => {
        // Semicolon insertion is needed to prevent ASI issues with computed property

        it('should handle computed property after regular property', () => {
            const input = `class A {
    foo = 1
    private ["bar"] = 2;
}`;
            const expected = `class A {
    foo = 1
    ;       ["bar"] = 2;
}`;
            assert.strictEqual(strip(input), expected);
        });

        it('should insert semicolon for public getter with computed property', () => {
            // public get [Symbol.toStringTag]() -> ; get [Symbol.toStringTag]()
            const input = 'class A { public get [Symbol.toStringTag]() { return "A"; } }';
            const expected = 'class A { ;      get [Symbol.toStringTag]() { return "A"; } }';
            assert.strictEqual(strip(input), expected);
        });

        it('should insert semicolon for public setter with computed property', () => {
            const input = 'class A { public set [key](v) { } }';
            const expected = 'class A { ;      set [key](v) { } }';
            assert.strictEqual(strip(input), expected);
        });

        it('should not insert semicolon for static getter with computed property', () => {
            // static is JS valid, no semicolon needed
            const input = 'class A { public static get [key]() { return 1; } }';
            const expected = 'class A {        static get [key]() { return 1; } }';
            assert.strictEqual(strip(input), expected);
        });

    });

    describe('Export Function Overload', () => {
        // Both fibjs and amaro erase function overload without body

        it('should erase export function overload signature without body', () => {
            const input = 'export function f();';
            const output = strip(input);
            assert.strictEqual(output.length, input.length);
            assert.strictEqual(output.trim(), '');
        });

    });

    describe('Static Keyword Context', () => {
        // Semicolon is needed after property named "static/get/set" to prevent ASI issues

        it('should handle static as property name', () => {
            const input = `class foo {
    static: any
    foo() { }
}`;
            const expected = `class foo {
    static;    
    foo() { }
}`;
            assert.strictEqual(strip(input), expected);
        });

        it('should handle get as property name', () => {
            const input = `class A { get: any }`;
            const expected = `class A { get;     }`;
            assert.strictEqual(strip(input), expected);
        });

        it('should handle set as property name', () => {
            const input = `class A { set: any }`;
            const expected = `class A { set;     }`;
            assert.strictEqual(strip(input), expected);
        });

        it('should not insert semicolon for regular property names', () => {
            const input = `class A { bar: T\n}`;
            const expected = `class A { bar   \n}`;
            assert.strictEqual(strip(input), expected);
        });

        it('should insert semicolon at type position even when property already has one', () => {
            // When property name is static/get/set, semicolon is inserted at type position
            // Original semicolon is preserved, resulting in two semicolons
            const input = `class A { static: any; }`;
            const expected = `class A { static;    ; }`;
            assert.strictEqual(strip(input), expected);
        });

        it('should not insert semicolon when property has initializer', () => {
            const input = `class A { static: any = 1 }`;
            const expected = `class A { static      = 1 }`;
            assert.strictEqual(strip(input), expected);
        });

    });

    describe('Declare Class', () => {

        it('should remove declare class', () => {
            const input = `declare class MyClass {
    constructor();
    method(): void;
}`;
            const expected = `                       
                  
                   
 `;
            assert.strictEqual(strip(input), expected);
        });

    });

    describe('Import/Export Equals (CommonJS)', () => {
        // amaro doesn't support import/export = syntax in strip-only mode

        itThrowsDiff('should preserve import equals (not erasable)',
            'import foo = require("foo");',
            null, // fibjs: succeeds (preserves it)
            /import equals.*not supported/); // amaro: throws

        itThrowsDiff('should preserve export equals (not erasable)',
            'export = 1;',
            null, // fibjs: succeeds (preserves it)
            /export assignment.*not supported/); // amaro: throws

    });

    describe('Angle Bracket Type Assertion (Not Supported)', () => {
        // amaro rejects angle bracket assertions in strip-only mode

        itThrowsDiff('should preserve angle bracket assertion (not erasable without transform)',
            'const foo = <number>1;',
            null, // fibjs: succeeds (preserves it)
            /angle-bracket syntax.*not supported/); // amaro: throws

    });

    describe('Private Class Fields', () => {

        it('should strip private field method overloads', () => {
            const input = `class Private {
    #foo(bar: string): string;
    #foo(bar: number): number;
    #foo(bar: string | number): string | number {
        return bar;
    }
}`;
            // Private method overload signatures should be completely erased
            const expected = `class Private {
                              
                              
    #foo(bar                 )                  {
        return bar;
    }
}`;
            assert.strictEqual(strip(input), expected);
        });

    });

    describe('Accessor Keyword', () => {

        it('should strip accessor keyword type annotation', () => {
            assert.strictEqual(strip('class Test { accessor name: string = ""; }'), 'class Test { accessor name         = ""; }');
        });

    });

    describe('Index Signature', () => {
        // Both fibjs and amaro strip index signatures in class

        it('should strip index signature in class', () => {
            assert.strictEqual(strip('class Test { [key: string]: any; }'), 'class Test {                     }');
        });

        it('should strip readonly index signature in class', () => {
            // readonly [x: string]: Object should be entirely removed without extra semicolon
            const input = 'class C { readonly [x: string]: Object; }';
            const expected = 'class C {                               }';
            assert.strictEqual(strip(input), expected);
        });

        it('should strip index signature with multiple modifiers', () => {
            const input = 'class C { public readonly [x: string]: any; }';
            const expected = 'class C {                                   }';
            assert.strictEqual(strip(input), expected);
        });

    });

    // ========================================
    // JavaScript Syntax Tolerance Tests
    // Parser should be lenient with JS syntax and only strip TS-specific parts
    // ========================================

    describe('JavaScript Syntax Tolerance', () => {
        // amaro is stricter about syntax

        itDiff('should handle incomplete code',
            'const x: number =',
            'const x         =',  // fibjs: strips type, preserves structure
            null);  // amaro: throws

        it('should handle missing semicolons', () => {
            assert.strictEqual(strip('const x: number = 1\nconst y: string = "a"'), 'const x         = 1\nconst y         = "a"');
        });

        it('should handle trailing comma', () => {
            assert.strictEqual(strip('function foo(a: number,) {}'), 'function foo(a        ,) {}');
        });

        it('should handle empty object', () => {
            assert.strictEqual(strip('const obj: {} = {}'), 'const obj     = {}');
        });

        it('should handle empty array', () => {
            assert.strictEqual(strip('const arr: [] = []'), 'const arr     = []');
        });

        it('should handle nested template literals', () => {
            const input = 'const x: string = `hello ${`nested ${a}`}`';
            const expected = 'const x         = `hello ${`nested ${a}`}`';
            assert.strictEqual(strip(input), expected);
        });

        it('should handle regex that looks like division', () => {
            assert.strictEqual(strip('const re: RegExp = /a/g'), 'const re         = /a/g');
        });

        it('should handle regex with special chars', () => {
            assert.strictEqual(strip('const re: RegExp = /[<>]/g'), 'const re         = /[<>]/g');
        });

        it('should handle multiple statements on one line', () => {
            assert.strictEqual(strip('let a: number = 1; let b: string = "x"'), 'let a         = 1; let b         = "x"');
        });

        it('should handle spread operator', () => {
            assert.strictEqual(strip('const arr: number[] = [...other]'), 'const arr           = [...other]');
        });

        it('should handle rest parameters', () => {
            assert.strictEqual(strip('function foo(...args: number[]) {}'), 'function foo(...args          ) {}');
        });

        it('should handle computed property names', () => {
            assert.strictEqual(strip('const obj = { [key]: value as string }'), 'const obj = { [key]: value           }');
        });

        it('should handle shorthand property', () => {
            assert.strictEqual(strip('const obj: Obj = { x, y }'), 'const obj      = { x, y }');
        });

        it('should handle method shorthand', () => {
            assert.strictEqual(strip('const obj = { foo(): void {} }'), 'const obj = { foo()       {} }');
        });

        it('should handle getter/setter', () => {
            assert.strictEqual(strip('const obj = { get x(): number { return 1; } }'), 'const obj = { get x()         { return 1; } }');
        });

        it('should handle async method', () => {
            assert.strictEqual(strip('const obj = { async foo(): Promise<void> {} }'), 'const obj = { async foo()                {} }');
        });

        it('should handle generator function', () => {
            assert.strictEqual(strip('function* gen(): Generator<number> { yield 1; }'), 'function* gen()                    { yield 1; }');
        });

        it('should handle async generator', () => {
            assert.strictEqual(strip('async function* gen(): AsyncGenerator<number> { yield 1; }'), 'async function* gen()                         { yield 1; }');
        });

        it('should handle nullish coalescing', () => {
            assert.strictEqual(strip('const x: number = a ?? b'), 'const x         = a ?? b');
        });

        it('should handle optional chaining', () => {
            assert.strictEqual(strip('const x: number = obj?.foo?.bar'), 'const x         = obj?.foo?.bar');
        });

        it('should handle logical assignment', () => {
            assert.strictEqual(strip('let x: number; x ||= 1'), 'let x        ; x ||= 1');
        });

        it('should handle class with static block', () => {
            const input = 'class Foo { static { console.log(1); } value: number = 0; }';
            const expected = 'class Foo { static { console.log(1); } value         = 0; }';
            assert.strictEqual(strip(input), expected);
        });

        it('should handle private field', () => {
            assert.strictEqual(strip('class Foo { #value: number = 0; }'), 'class Foo { #value         = 0; }');
        });

        it('should handle decorators (preserved)', () => {
            const input = '@decorator class Foo { @prop value: number = 0; }';
            const expected = '@decorator class Foo { @prop value         = 0; }';
            assert.strictEqual(strip(input), expected);
        });

        it('should strip type assertions in decorator arguments', () => {
            const input = '@y(1 as T, () => C) class C<T> {}';
            const expected = '@y(1     , () => C) class C    {}';
            assert.strictEqual(strip(input), expected);
        });

        it('should strip type assertions in method decorator arguments', () => {
            const input = `class C {
    @y(null as T)
    method() {}
}`;
            const expected = `class C {
    @y(null     )
    method() {}
}`;
            assert.strictEqual(strip(input), expected);
        });

    });

    describe('Edge Cases and Unusual Patterns', () => {

        it('should not strip comparison operators that look like generics', () => {
            // Bug: a < b && c > d was being stripped as if < b && c > was a generic
            const input = 'if (a < b && c > d) {}';
            const expected = 'if (a < b && c > d) {}';
            assert.strictEqual(strip(input), expected);
        });

        it('should not strip less than followed by identifier and greater than', () => {
            // More complex comparison expressions should be preserved
            const input = 'if (spaceBelow < dropdownHeight && spaceAbove > spaceBelow) {}';
            const expected = 'if (spaceBelow < dropdownHeight && spaceAbove > spaceBelow) {}';
            assert.strictEqual(strip(input), expected);
        });

        it('should handle type annotation after line break', () => {
            const input = `const x
: number = 1`;
            const expected = `const x
         = 1`;
            assert.strictEqual(strip(input), expected);
        });

        it('should handle multiple colons in ternary', () => {
            assert.strictEqual(strip('const x: number = a ? b : c'), 'const x         = a ? b : c');
        });

        it('should handle nested ternary', () => {
            assert.strictEqual(strip('const x: number = a ? b ? c : d : e'), 'const x         = a ? b ? c : d : e');
        });

        it('should handle object with type-like property names', () => {
            // Note: 'interface' and 'as' are TypeScript keywords, parser may treat them specially
            // This is a known limitation - use quoted property names if needed
            assert.strictEqual(strip('const obj = { type: 1, "interface": 2, "as": 3 }'), 'const obj = { type: 1, "interface": 2, "as": 3 }');
        });

        it('should handle label statement', () => {
            assert.strictEqual(strip('outer: for (let i: number = 0;;) break outer'), 'outer: for (let i         = 0;;) break outer');
        });

        it('should handle switch case with colon', () => {
            const input = 'switch (x as number) { case 1: break; }';
            const expected = 'switch (x          ) { case 1: break; }';
            assert.strictEqual(strip(input), expected);
        });

        it('should not strip switch case label colon', () => {
            const input = 'switch (x) { case Foo.Bar: break; default: break; }';
            assert.strictEqual(strip(input), input);
        });

        it('should handle object destructuring with rename and type', () => {
            // Destructuring with rename { a: b } - correctly preserves rename, strips only type annotation
            assert.strictEqual(strip('const { a: b }: Obj = obj'), 'const { a: b }      = obj');
        });

        it('should handle object destructuring with default and type', () => {
            // Destructuring { a = 1 }: Obj - correctly strips type annotation after }
            assert.strictEqual(strip('const { a = 1 }: Obj = obj'), 'const { a = 1 }      = obj');
        });

        it('should handle array destructuring with skip', () => {
            assert.strictEqual(strip('const [, second]: [number, number] = arr'), 'const [, second]                   = arr');
        });

        itDiff('should handle for-of with type',
            'for (const x: number of arr) {}',
            'for (const x         of arr) {}', // fibjs: strips type
            null); // amaro: throws

        itDiff('should handle for-in with type',
            'for (const k: string in obj) {}',
            'for (const k         in obj) {}', // fibjs: strips type
            null); // amaro: throws

        it('should handle arrow function without parens', () => {
            assert.strictEqual(strip('const fn = x => x as number'), 'const fn = x => x          ');
        });

        it('should handle class expression', () => {
            assert.strictEqual(strip('const C = class implements I {}'), 'const C = class              {}');
        });

        it('should handle IIFE with type', () => {
            assert.strictEqual(strip('(function(): void {})()'), '(function()       {})()');
        });

        it('should handle empty source', () => {
            assert.strictEqual(strip(''), '');
        });

        it('should handle whitespace only', () => {
            assert.strictEqual(strip('   \n\t  '), '   \n\t  ');
        });

        // fibjs erases all comments; amaro preserves them
        itDiff('should handle comments only',
            '// comment\n/* block */',
            '          \n           ',  // fibjs: comments erased
            '// comment\n/* block */'   // amaro: comments preserved
        );

        it('should handle string with type-like content', () => {
            assert.strictEqual(strip('const s: string = "const x: number = 1"'), 'const s         = "const x: number = 1"');
        });

        it('should handle template literal with type-like content', () => {
            assert.strictEqual(strip('const s: string = `type Foo = ${bar}`'), 'const s         = `type Foo = ${bar}`');
        });

        it('should handle consecutive type annotations', () => {
            // All type annotations are stripped, including the last one
            assert.strictEqual(strip('let a: A, b: B, c: C'), 'let a   , b   , c   ');
        });

        it('should handle function with many parameters', () => {
            const input = 'function f(a: A, b: B, c: C, d: D): R {}';
            const expected = 'function f(a   , b   , c   , d   )    {}';
            assert.strictEqual(strip(input), expected);
        });

        it('should handle deeply nested generics', () => {
            assert.strictEqual(strip('const x: A<B<C<D<E>>>> = v'), 'const x                = v');
        });

        it('should handle union with many types', () => {
            // Type annotation is correctly stripped even at end of input
            assert.strictEqual(strip('let x: A | B | C | D | E'), 'let x                   ');
        });

        it('should handle intersection with many types', () => {
            assert.strictEqual(strip('let x: A & B & C & D & E'), 'let x                   ');
        });

    });

    describe('Malformed Code Tolerance', () => {
        // These tests use intentionally malformed syntax that amaro rejects
        // fibjs tolerates them and strips types where possible, preserving length

        itDiff('should handle unclosed brace (partial class)',
            'class Foo { x: number',
            'class Foo { x        ',  // fibjs: strips type annotation
            null);  // amaro: throws

        itDiff('should handle unclosed paren (partial function)',
            'function foo(x: number',
            '                      ',  // fibjs: erases as incomplete function signature
            null);  // amaro: throws

        itDiff('should handle extra closing brace',
            'const x: number = 1; }',
            'const x         = 1; }',  // fibjs: strips type, keeps extra brace
            null);  // amaro: throws

        itDiff('should handle mismatched brackets',
            'const x: number = [1, 2)',
            'const x         = [1, 2)',  // fibjs: strips type, keeps mismatched bracket
            null);  // amaro: throws

        itDiff('should handle double colon (invalid but tolerated)',
            'const x:: number = 1',
            'const x : number = 1',  // fibjs: erases first colon only
            null);  // amaro: throws

        itDiff('should handle consecutive operators',
            'const x: number = 1 ++ --',
            'const x         = 1 ++ --',  // fibjs: strips type, keeps invalid operators
            null);  // amaro: throws

        itDiff('should handle random keywords',
            'const x: number = class function',
            'const x         = class         ',  // fibjs: strips type, erases dangling function
            null);  // amaro: throws

        itDiff('should handle truncated type alias',
            'type T =',
            '        ',  // fibjs: erases entire type alias
            null);  // amaro: throws

        itDiff('should handle truncated interface',
            'interface A {',
            '             ',  // fibjs: erases entire interface
            null);  // amaro: throws

        itDiff('should handle truncated object type annotation',
            'const x: { a: number = 1',
            'const x                 ',  // fibjs: erases type annotation including truncated object type
            null);  // amaro: throws

        itDiff('should handle missing identifier before type annotation',
            'const : number = 1',
            'const          = 1',  // fibjs: strips type annotation
            null);  // amaro: throws

        itDiff('should handle unterminated string literal',
            'const s: string = "unterminated',
            'const s         = "unterminated',  // fibjs: strips type, keeps unterminated string
            null);  // amaro: throws

        itDiff('should handle unterminated template literal',
            'const s: string = `unterminated ${x}',
            'const s         = `unterminated ${x}',  // fibjs: strips type, keeps unterminated template
            null);  // amaro: throws

        itDiff('should handle unterminated block comment',
            'const x: number = 1; /* unterminated',
            'const x         = 1;                ',  // fibjs: strips type, erases unterminated comment
            null);  // amaro: throws

        itDiff('should handle unterminated regex literal',
            'const re: RegExp = /unterminated',
            'const re         = /unterminated',  // fibjs: strips type, keeps unterminated regex
            null);  // amaro: throws

        itDiff('should handle incomplete hex escape in string',
            'const s: string = "\\x";',
            'const s         = "\\x";',  // fibjs: strips type, keeps invalid escape
            null);  // amaro: throws

        itDiff('should handle incomplete unicode escape in string',
            'const s: string = "\\u{";',
            'const s         = "\\u{";',  // fibjs: strips type, keeps invalid escape
            null);  // amaro: throws

        itDiff('should handle truncated generic call (missing >)',
            'foo<string(1);',
            'foo<string(1);',  // fibjs: preserves as-is (not recognized as type args)
            null);  // amaro: throws

        itDiff('should handle truncated arrow function (missing >)',
            'const fn: (x: number) => number = (x: number) = x;',
            'const fn                        = (x        ) = x;',  // fibjs: strips type annotations
            null);  // amaro: throws

        itDiff('should handle truncated tuple type',
            'const t: [number, string = [1, "a"];',
            'const t                             ',  // fibjs: erases entire type annotation
            null);  // amaro: throws

        itDiff('should handle truncated conditional type',
            'type X<T> = T extends string ? 1 :',
            '                                  ',  // fibjs: erases entire type alias
            null);  // amaro: throws

        itDiff('should handle truncated mapped type',
            'type M<T> = { [K in keyof T]:',
            '                             ',  // fibjs: erases entire type alias
            null);  // amaro: throws

        itDiff('should handle truncated import statement',
            'import { a } from "mod"',
            'import { a } from "mod"',  // fibjs: preserves valid import (just missing semicolon)
            null);  // amaro: throws

        itDiff('should handle truncated export statement',
            'export { a',
            'export { a',  // fibjs: preserves as-is (incomplete but no types)
            null);  // amaro: throws

        itDiff('should handle truncated object literal after as assertion',
            'const x = ({ a: 1 } as { a: number',
            'const x = ({ a: 1 }               ',  // fibjs: strips as assertion including truncated type
            null);  // amaro: throws

    });

    describe('Comments Handling', () => {
        // fibjs erases all comments at scanner level for consistency
        // amaro preserves comments (only erases type annotations)

        itDiff('should handle comment after interface',
            'interface A { x: number }\n\n// comment',
            '                         \n\n          ',  // fibjs: comments erased
            '                         \n\n// comment'   // amaro: comments preserved
        );

        itDiff('should handle Chinese comment after interface',
            'interface A { x: number }\n\n// 中文注释',
            '                         \n\n               ',  // fibjs: comments erased (15 bytes: // + 4 chinese chars * 3)
            '                         \n\n// 中文注释'   // amaro: comments preserved
        );

        itDiff('should handle comment after type alias',
            'type A = string;\n\n// comment',
            '                \n\n          ',  // fibjs: comments erased
            '                \n\n// comment'   // amaro: comments preserved
        );

        itDiff('should handle comment after declare',
            'declare const x: number;\n\n// comment',
            '                        \n\n          ',  // fibjs: comments erased
            '                        \n\n// comment'   // amaro: comments preserved
        );

        itDiff('should handle block comment after interface',
            'interface A {}\n\n/* block comment */',
            '              \n\n                   ',  // fibjs: comments erased
            '              \n\n/* block comment */'   // amaro: comments preserved
        );

        itDiff('should handle comment between interfaces',
            'interface A {}\n// comment\ninterface B {}',
            '              \n          \n              ',  // fibjs: comments erased
            '              \n// comment\n              '   // amaro: comments preserved
        );

        itDiff('should handle multiple comments after interface',
            'interface A {}\n// line1\n// line2\nconst x = 1;',
            '              \n        \n        \nconst x = 1;',  // fibjs: comments erased
            '              \n// line1\n// line2\nconst x = 1;'   // amaro: comments preserved
        );

        itDiff('should handle comment after interface with body',
            'interface Foo {\n    x: number;\n}\n// This comment',
            '               \n              \n \n               ',  // fibjs: comments erased
            '               \n              \n \n// This comment'   // amaro: comments preserved
        );

    });

    describe('Real World Issues', () => {

        describe('Function Parameter Types', () => {

            it('should strip function parameter type in regular function', () => {
                const input = 'function getAppName(type: string): string { return type; }';
                const expected = 'function getAppName(type        )         { return type; }';
                assert.strictEqual(strip(input), expected);
            });

            it('should strip multiple function parameter types', () => {
                const input = 'function add(a: number, b: number): number { return a + b; }';
                const expected = 'function add(a        , b        )         { return a + b; }';
                assert.strictEqual(strip(input), expected);
            });

            it('should strip arrow function parameter type', () => {
                const input = 'const fn = (x: number) => x * 2;';
                const expected = 'const fn = (x        ) => x * 2;';
                assert.strictEqual(strip(input), expected);
            });

            it('should strip arrow function parameter type with return type', () => {
                const input = 'const fn = (x: number): number => x * 2;';
                const expected = 'const fn = (x        )         => x * 2;';
                assert.strictEqual(strip(input), expected);
            });

            it('should strip arrow function parameter type with default value and return type', () => {
                const input = 'const avatar = (options: AvatarPluginOptions = {}): BetterAuthPlugin => {}';
                const expected = 'const avatar = (options                      = {})                   => {}';
                assert.strictEqual(strip(input), expected);
            });

            it('should strip function-type parameter and return types', () => {
                const input = 'export function addScrollListener(handler: () => void): () => void { return handler; }';
                const out = strip(input);
                // No type-level `=>` should remain.
                assert.ok(!out.includes('=> void'), out);
                assert.ok(out.includes('addScrollListener(handler'), out);
            });

            it('should strip string-literal union type in parameter', () => {
                const input = "function f(x: 'unhealthy' | 'unreachable') { return x; }";
                const out = strip(input);
                assert.ok(!out.includes("'unhealthy'"), out);
                assert.ok(!out.includes("'unreachable'"), out);
                assert.ok(out.includes('function f(x'), out);
            });

            it('should strip string-literal type in optional parameter', () => {
                const input = "function g(kind?: 'Job') { return kind; }";
                const out = strip(input);
                assert.ok(!out.includes("'Job'"), out);
                assert.ok(out.includes('function g(kind'), out);
            });

            it('should strip string-literal union type in variable declaration', () => {
                const input = "let statusType: 'notready' | 'scheduling-disabled' | 'pressure' = 'ready';";
                const out = strip(input);
                assert.ok(!out.includes("'notready'"), out);
                assert.ok(!out.includes("'scheduling-disabled'"), out);
                assert.ok(!out.includes("'pressure'"), out);
                assert.ok(out.includes("= 'ready'"), out);
            });

        });

        describe('Class Access Modifiers', () => {

            it('should strip private modifier', () => {
                const input = 'class A { private x = 1; }';
                const expected = 'class A {         x = 1; }';
                assert.strictEqual(strip(input), expected);
            });

            it('should strip public modifier', () => {
                const input = 'class A { public x = 1; }';
                const expected = 'class A {        x = 1; }';
                assert.strictEqual(strip(input), expected);
            });

            it('should strip protected modifier', () => {
                const input = 'class A { protected x = 1; }';
                const expected = 'class A {           x = 1; }';
                assert.strictEqual(strip(input), expected);
            });

            it('should strip private static', () => {
                const input = 'class A { private static instance: A; }';
                const expected = 'class A {         static instance   ; }';
                assert.strictEqual(strip(input), expected);
            });

            it('should strip private constructor', () => {
                const input = 'class A { private constructor() {} }';
                const expected = 'class A {         constructor() {} }';
                assert.strictEqual(strip(input), expected);
            });

            it('should strip private async method after nested class', () => {
                const input =
                    'class A {\n'
                    + '  m() { class Inner {} }\n'
                    + '  private async loadLanguageFile(langPath: string): Promise<any> { return 1; }\n'
                    + '}';

                const out = strip(input);
                assert.ok(!out.includes('private async'), out);
                assert.ok(out.includes('async loadLanguageFile(langPath        )'), out);
                assert.ok(!out.includes('): Promise'), out);
            });

            it('should strip private async method after template literal expression', () => {
                const input =
                    'class A {\n'
                    + '  m(a: string) { const x = `${a}/${a}.js`; return x; }\n'
                    + '  private async loadLanguageFile(langPath: string): Promise<any> { return 1; }\n'
                    + '}';

                const out = strip(input);
                assert.ok(!out.includes('private async'), out);
                assert.ok(out.includes('async loadLanguageFile(langPath        )'), out);
            });

            it('should strip public static method return type', () => {
                const input = 'class A { public static getInstance(): A { return new A(); } }';
                const expected = 'class A {        static getInstance()    { return new A(); } }';
                assert.strictEqual(strip(input), expected);
            });

            it('should strip class method union return type', () => {
                const input = 'class A { getRenderer(language: string): Foo | null { return null; } }';
                const out = strip(input);
                assert.ok(out.includes('getRenderer(language        )'), out);
                assert.ok(!out.includes('): Foo'), out);
            });

        });

        describe('Object Literal Type Annotations', () => {

            it('should strip type annotation in object property shorthand context', () => {
                const input = 'const obj = { display: "flex", justifyContent: "center" };';
                const expected = 'const obj = { display: "flex", justifyContent: "center" };';
                assert.strictEqual(strip(input), expected);
            });

            it('should not confuse object property with type annotation', () => {
                const input = 'const style = { display: value, color: "red" };';
                const expected = 'const style = { display: value, color: "red" };';
                assert.strictEqual(strip(input), expected);
            });

            it('should preserve object property with contextual keyword name and newline value', () => {
                const input = 'const obj = {\n    type:\n        libMap,\n};';
                assert.strictEqual(strip(input), input);
            });

            it('should preserve object literal boolean value inside call args', () => {
                const input = 'use({ nonRecursive: true, dir: rootDir });';
                assert.strictEqual(strip(input), input);
            });

            it('should preserve computed property key in object literal', () => {
                const input = 'const obj = { [k]: v };';
                assert.strictEqual(strip(input), input);
            });

            it('should preserve object destructuring rename', () => {
                const input = 'const { fileNames: projects, errors } = x;';
                assert.strictEqual(strip(input), input);
            });

            it('should preserve object property value with bracket indexing', () => {
                const input = 'const obj = { path: match[0] };';
                assert.strictEqual(strip(input), input);
            });

            it('should preserve object literal after logical operators', () => {
                const input = 'const x = cond && { a: value, computedWithoutCache: false } || { a: other };';
                const out = strip(input);
                assert.strictEqual(normalize(out), normalize(input));
            });

            it('should preserve object literal after nullish coalescing assignment', () => {
                const input = 'x ??= { facts: ClassFacts.None, y: 1 };';
                const out = strip(input);
                assert.strictEqual(normalize(out), normalize(input));
            });

        });

        describe('Ternary Operator vs Type Annotation', () => {

            it('should not strip ternary colon before array literal', () => {
                const input = 'function reg(languages: string | string[]) { const languageList = Array.isArray(languages) ? languages : [languages]; }';
                const out = strip(input);
                // Ensure ternary ':' is preserved; otherwise it becomes `? languages ;` and breaks JS parsing.
                assert.ok(out.includes('? languages : [languages];'), out);
            });

        });

        describe('Export with Types', () => {

            it('should strip export interface', () => {
                const input = 'export interface Props { name: string; }';
                // Entire export interface is erased to avoid orphan 'export'
                const expected = '                                        ';
                assert.strictEqual(strip(input), expected);
            });

            it('should strip export type', () => {
                const input = 'export type ID = string | number;';
                // Entire export type is erased to avoid orphan 'export'
                const expected = '                                 ';
                assert.strictEqual(strip(input), expected);
            });

            it('should keep export const with type', () => {
                const input = 'export const x: number = 1;';
                const expected = 'export const x         = 1;';
                assert.strictEqual(strip(input), expected);
            });

            it('should keep export function with types stripped', () => {
                const input = 'export function foo(x: number): string { return String(x); }';
                const expected = 'export function foo(x        )         { return String(x); }';
                assert.strictEqual(strip(input), expected);
            });

            it('should strip abstract keyword in export abstract class', () => {
                const input = 'export abstract class BaseChartTool { abstract run(): void; }';
                const out = strip(input);
                assert.ok(!out.includes('export abstract class'), out);
                assert.ok(out.includes('export          class BaseChartTool'), out);
            });

        });

        describe('Async Functions with Types', () => {

            it('should strip async function parameter and return types', () => {
                const input = 'async function fetch(url: string): Promise<Response> { }';
                const expected = 'async function fetch(url        )                    { }';
                assert.strictEqual(strip(input), expected);
            });

            it('should strip async arrow function types', () => {
                const input = 'const fetch = async (url: string): Promise<Response> => { };';
                const expected = 'const fetch = async (url        )                    => { };';
                assert.strictEqual(strip(input), expected);
            });

            it('should strip multiline async class method parameter and return types', () => {
                const input =
                    'class A { async listAttachments(\n' +
                    '    workspaceId: number,\n' +
                    '    filetype?: string,\n' +
                    '    startId: number = Number.MAX_SAFE_INTEGER,\n' +
                    '    limit: number = 100\n' +
                    '): Promise<AttachmentsResponse> { return 1; } }';

                const expected =
                    'class A { async listAttachments(\n' +
                    '    workspaceId        ,\n' +
                    '    filetype         ,\n' +
                    '    startId         = Number.MAX_SAFE_INTEGER,\n' +
                    '    limit         = 100\n' +
                    ')                               { return 1; } }';

                assert.strictEqual(strip(input), expected);
            });

            it('should not break type stripping after template literals in previous method body', () => {
                const input =
                    'class A {\n' +
                    '    async uploadAttachment(workspaceId: number, data: UploadData): Promise<Resp> {\n' +
                    '        const response = await fetch(`/api/${workspaceId}`, {\n' +
                    '            headers: {\n' +
                    '                \'X\': encodeURIComponent(data.name)\n' +
                    '            }\n' +
                    '        });\n' +
                    '        return response;\n' +
                    '    }\n' +
                    '\n' +
                    '    async listAttachments(\n' +
                    '        workspaceId: number,\n' +
                    '        filetype?: string,\n' +
                    '        startId: number = Number.MAX_SAFE_INTEGER,\n' +
                    '        limit: number = 100\n' +
                    '    ): Promise<AttachmentsResponse> { return 1; }\n' +
                    '}';

                const expected =
                    'class A {\n' +
                    '    async uploadAttachment(workspaceId        , data            )                {\n' +
                    '        const response = await fetch(`/api/${workspaceId}`, {\n' +
                    '            headers: {\n' +
                    '                \'X\': encodeURIComponent(data.name)\n' +
                    '            }\n' +
                    '        });\n' +
                    '        return response;\n' +
                    '    }\n' +
                    '\n' +
                    '    async listAttachments(\n' +
                    '        workspaceId        ,\n' +
                    '        filetype         ,\n' +
                    '        startId         = Number.MAX_SAFE_INTEGER,\n' +
                    '        limit         = 100\n' +
                    '    )                               { return 1; }\n' +
                    '}';

                assert.strictEqual(strip(input), expected);
            });

        });

        describe('Variable Declarations without Initializer', () => {

            it('should strip type from uninitialized variable', () => {
                const input = 'let x: number;';
                const expected = 'let x        ;';
                assert.strictEqual(strip(input), expected);
            });

            it('should strip type from class property without initializer', () => {
                const input = 'class A { x: number; }';
                const expected = 'class A { x        ; }';
                assert.strictEqual(strip(input), expected);
            });

            it('should strip union type from class property without initializer', () => {
                const input = 'class A { agent: Agent | null; }';
                const expected = 'class A { agent              ; }';
                assert.strictEqual(strip(input), expected);
            });

        });

        describe('Readonly and Other Modifiers', () => {

            it('should strip readonly in class', () => {
                const input = 'class A { readonly x: number = 1; }';
                const expected = 'class A {          x         = 1; }';
                assert.strictEqual(strip(input), expected);
            });

            it('should strip private readonly', () => {
                const input = 'class A { private readonly x: number = 1; }';
                const expected = 'class A {                  x         = 1; }';
                assert.strictEqual(strip(input), expected);
            });

        });

        describe('Contextual Keywords as Method Names with Generics', () => {

            it('should strip get method generic parameter', () => {
                // 'get' is a keyword but can be used as method name
                const input = 'class A { get<T>(key: string): T { return null; } }';
                const expected = 'class A { get   (key        )    { return null; } }';
                assert.strictEqual(strip(input), expected);
            });

            it('should strip set method generic parameter', () => {
                const input = 'class A { set<T>(key: string, value: T): void { } }';
                const expected = 'class A { set   (key        , value   )       { } }';
                assert.strictEqual(strip(input), expected);
            });

            it('should strip type method generic parameter', () => {
                // 'type' as method name
                const input = 'class A { type<T>(value: T): string { return ""; } }';
                const expected = 'class A { type   (value   )         { return ""; } }';
                assert.strictEqual(strip(input), expected);
            });

            it('should strip async method generic parameter', () => {
                const input = 'class A { async fetch<T>(url: string): Promise<T> { } }';
                const expected = 'class A { async fetch   (url        )             { } }';
                assert.strictEqual(strip(input), expected);
            });

        });

        describe('Contextual Keywords as Function Names', () => {

            it('should preserve satisfies as function name', () => {
                // 'satisfies' is a contextual keyword but can be used as function name
                const input = 'function satisfies(a, b) { return a === b; }';
                const expected = 'function satisfies(a, b) { return a === b; }';
                assert.strictEqual(strip(input), expected);
            });

            it('should preserve satisfies as function name with types', () => {
                const input = 'function satisfies(a: number, b: number): boolean { return a === b; }';
                const expected = 'function satisfies(a        , b        )          { return a === b; }';
                assert.strictEqual(strip(input), expected);
            });

        });

        describe('Contextual Keywords in Object Binding Pattern', () => {

            it('should preserve get in object destructuring parameter', () => {
                const input = 'function f({ get }) {}';
                const expected = 'function f({ get }) {}';
                assert.strictEqual(strip(input), expected);
            });

            it('should preserve set in object destructuring parameter', () => {
                const input = 'function f({ set }) {}';
                const expected = 'function f({ set }) {}';
                assert.strictEqual(strip(input), expected);
            });

            it('should preserve multiple contextual keywords in destructuring', () => {
                const input = 'function f({ get, set }) {}';
                const expected = 'function f({ get, set }) {}';
                assert.strictEqual(strip(input), expected);
            });

            it('should preserve mixed identifiers and contextual keywords', () => {
                const input = 'function f({ a, get, b }) {}';
                const expected = 'function f({ a, get, b }) {}';
                assert.strictEqual(strip(input), expected);
            });

            it('should preserve contextual keywords with type annotation', () => {
                const input = 'function f({ get, set }: { get: number, set: number }) {}';
                const expected = 'function f({ get, set }                              ) {}';
                assert.strictEqual(strip(input), expected);
            });

        });

        describe('Type as Identifier in Import/Export', () => {

            it('should preserve type as identifier in export', () => {
                // In JS, 'type' can be a variable name
                const input = 'export { pool, exec_pool, type };';
                const expected = 'export { pool, exec_pool, type };';
                assert.strictEqual(strip(input), expected);
            });

            it('should preserve type as identifier in import', () => {
                const input = 'import { exec_pool, type } from "./db.js";';
                const expected = 'import { exec_pool, type } from "./db.js";';
                assert.strictEqual(strip(input), expected);
            });

            it('should preserve type with as clause in export', () => {
                const input = 'export { type as myType };';
                const expected = 'export { type as myType };';
                assert.strictEqual(strip(input), expected);
            });

            it('should preserve type with as clause in import', () => {
                const input = 'import { type as myType } from "./mod.js";';
                const expected = 'import { type as myType } from "./mod.js";';
                assert.strictEqual(strip(input), expected);
            });

            it('should strip TypeScript type modifier in export', () => {
                // TypeScript: export { type X } means type-only export
                const input = 'export { type User, getData };';
                const expected = 'export {            getData };';
                assert.strictEqual(strip(input), expected);
            });

            it('should strip TypeScript type modifier in import', () => {
                const input = 'import { type User, getData } from "./mod";';
                const expected = 'import {            getData } from "./mod";';
                assert.strictEqual(strip(input), expected);
            });

            it('should handle mixed type identifiers and type modifiers', () => {
                // 'type' alone is identifier, 'type X' is modifier
                const input = 'export { type, type User };';
                const expected = 'export { type,           };';
                assert.strictEqual(strip(input), expected);
            });

            it('should preserve type as default import name', () => {
                // import type from './type.js' - 'type' is the default import name
                const input = "import type from './type.js';";
                const expected = "import type from './type.js';";
                assert.strictEqual(strip(input), expected);
            });

            it('should preserve type as default import with named imports', () => {
                // import type, { foo } from './mod.js'
                const input = "import type, { foo } from './mod.js';";
                const expected = "import type, { foo } from './mod.js';";
                assert.strictEqual(strip(input), expected);
            });

            it('should strip TypeScript import type (not default import)', () => {
                // import type X from '...' - TS type import
                const input = "import type X from './types';";
                const expected = "                             ";
                assert.strictEqual(strip(input), expected);
            });

        });

        describe('Shebang Preservation', () => {

            it('should preserve shebang at start of file', () => {
                const input = "#!/usr/bin/env node\nconsole.log(1);";
                const expected = "#!/usr/bin/env node\nconsole.log(1);";
                assert.strictEqual(strip(input), expected);
            });

            it('should preserve shebang with TypeScript code', () => {
                const input = "#!/usr/bin/env ts-node\nconst x: number = 1;";
                const expected = "#!/usr/bin/env ts-node\nconst x         = 1;";
                assert.strictEqual(strip(input), expected);
            });

        });

        describe('Contextual Keywords as Parameter Names', () => {

            it('should preserve override as parameter name', () => {
                const input = 'function simple(node, visitors, baseVisitor, state, override) {}';
                const expected = 'function simple(node, visitors, baseVisitor, state, override) {}';
                assert.strictEqual(strip(input), expected);
            });

            it('should preserve readonly as parameter name', () => {
                const input = 'function test(readonly) { return readonly; }';
                const expected = 'function test(readonly) { return readonly; }';
                assert.strictEqual(strip(input), expected);
            });

            it('should preserve public as parameter name', () => {
                const input = 'const fn = (public) => public + 1;';
                const expected = 'const fn = (public) => public + 1;';
                assert.strictEqual(strip(input), expected);
            });

            it('should strip public as constructor parameter modifier', () => {
                const input = 'class A { constructor(public name: string) {} }';
                const expected = 'class A { constructor(       name        ) {} }';
                assert.strictEqual(strip(input), expected);
            });

            it('should strip override as constructor parameter modifier', () => {
                const input = 'class A { constructor(override name: string) {} }';
                const expected = 'class A { constructor(         name        ) {} }';
                assert.strictEqual(strip(input), expected);
            });

        });

        describe('Export Interface and Type Complete Erasure', () => {

            it('should completely erase export interface', () => {
                const input = 'export interface User { name: string; }';
                const expected = '                                       ';
                assert.strictEqual(strip(input), expected);
            });

            it('should completely erase export type alias', () => {
                const input = 'export type ID = string | number;';
                const expected = '                                 ';
                assert.strictEqual(strip(input), expected);
            });

            it('should erase export interface with generics', () => {
                const input = 'export interface List<T> { items: T[]; }';
                const expected = '                                        ';
                assert.strictEqual(strip(input), expected);
            });

        });

        describe('Object Literal vs Type Annotation Disambiguation', () => {

            it('should preserve object property with identifier value', () => {
                // { key: value } is object literal, not type annotation
                const input = 'const obj = { name: userName, age: userAge };';
                const expected = 'const obj = { name: userName, age: userAge };';
                assert.strictEqual(strip(input), expected);
            });

            it('should preserve object property with string value', () => {
                const input = 'const style = { display: "flex", color: "red" };';
                const expected = 'const style = { display: "flex", color: "red" };';
                assert.strictEqual(strip(input), expected);
            });

            it('should preserve object property with number value', () => {
                const input = 'const config = { timeout: 1000, retries: 3 };';
                const expected = 'const config = { timeout: 1000, retries: 3 };';
                assert.strictEqual(strip(input), expected);
            });

            it('should preserve nested object literal', () => {
                const input = 'const obj = { outer: { inner: value } };';
                const expected = 'const obj = { outer: { inner: value } };';
                assert.strictEqual(strip(input), expected);
            });

            it('should strip type annotation in function parameter but preserve object in body', () => {
                const input = 'function f(x: number) { return { result: x }; }';
                const expected = 'function f(x        ) { return { result: x }; }';
                assert.strictEqual(strip(input), expected);
            });

            it('should strip arrow function return type in object property value', () => {
                // onActivate: (): void => { ... } - the `: void` should be stripped
                const input = 'const obj = { onClick: (): void => { doSomething(); } };';
                const expected = 'const obj = { onClick: ()       => { doSomething(); } };';
                assert.strictEqual(strip(input), expected);
            });

            it('should strip arrow function with params return type in object property value', () => {
                const input = 'const obj = { handler: (e: Event): void => { process(e); } };';
                const expected = 'const obj = { handler: (e       )       => { process(e); } };';
                assert.strictEqual(strip(input), expected);
            });

            it('should strip arrow function return type in nested callback object literal', () => {
                // Real world case: object literal inside map callback
                // The outer paren depth > 0 due to map(), but the inner object property should still work
                const input = 'items.map(x => { return { onActivate: (): void => { run(); } }; });';
                const expected = 'items.map(x => { return { onActivate: ()       => { run(); } }; });';
                assert.strictEqual(strip(input), expected);
            });

            it('should preserve multiple object properties with member access in callback', () => {
                // Regression test: multiple properties with dot notation values should not be erased
                const input = 'items.map(x => { return { title: childItem.title, name: childItem.name }; });';
                const expected = 'items.map(x => { return { title: childItem.title, name: childItem.name }; });';
                assert.strictEqual(strip(input), expected);
            });

            it('should strip arrow function return type in deeply nested object literal', () => {
                // Bug: two levels of object nesting, inner arrow function return type not stripped
                // Single level works: { b: () => { return { f: (): void => {} }; } }
                // Double level fails: { a: { b: () => { return { f: (): void => {} }; } } }
                const input = '{ a: { b: () => { return { f: (): void => {} }; } } }';
                const expected = '{ a: { b: () => { return { f: ()       => {} }; } } }';
                assert.strictEqual(strip(input), expected);
            });

            it('should preserve shorthand arrow function parameter in object literal', () => {
                // Bug: { fn: x => x + 1 } - the `: x` was being stripped as type annotation
                // but this is actually shorthand arrow function: fn: (x) => x + 1
                const input = 'const obj = { fn: x => x + 1 };';
                const expected = 'const obj = { fn: x => x + 1 };';
                assert.strictEqual(strip(input), expected);
            });

            itDiff('should preserve shorthand arrow function in return object literal',
                // Real world case from TypeScript source
                // amaro doesn't allow return statement outside function
                'return { hasResult: path => check(path) };',
                'return { hasResult: path => check(path) };', // fibjs: preserves
                null); // amaro: throws

            it('should preserve multiple shorthand arrow functions in function call object literal', () => {
                // Bug: second arrow function parameter was being stripped
                // f({ a: x => x, b: y => y }) was becoming f({ a: x => x, b    => y })
                const input = 'f({ a: x => x, b: y => y });';
                const expected = 'f({ a: x => x, b: y => y });';
                assert.strictEqual(strip(input), expected);
            });

            it('should preserve shorthand arrow function in nested function call', () => {
                // Inside parentheses (m_parenDepth > 0) with object literal
                const input = 'doSomething({ resolve: name => doResolve(name), retry: res => res.ok });';
                const expected = 'doSomething({ resolve: name => doResolve(name), retry: res => res.ok });';
                assert.strictEqual(strip(input), expected);
            });

            it('should preserve object property with simple identifier value', () => {
                // Most basic case: { key: value } should NOT be treated as type annotation
                const input = 'const obj = { foo: bar };';
                const expected = 'const obj = { foo: bar };';
                assert.strictEqual(strip(input), expected);
            });

            it('should preserve object property with undefined value', () => {
                // Bug: { performance: undefined } was erased to { performance            }
                const input = 'const obj = { performance: undefined };';
                const expected = 'const obj = { performance: undefined };';
                assert.strictEqual(strip(input), expected);
            });

            it('should preserve object destructuring assignment with renamed property', () => {
                // Bug: { helperFactory: emitHelpers } was erased incorrectly
                const input = 'const { helperFactory: emitHelpers } = context;';
                const expected = 'const { helperFactory: emitHelpers } = context;';
                assert.strictEqual(strip(input), expected);
            });

            it('should preserve object property with true/false value', () => {
                const input = 'const opts = { nonRecursive: true, enabled: false };';
                const expected = 'const opts = { nonRecursive: true, enabled: false };';
                assert.strictEqual(strip(input), expected);
            });

            itDiff('should preserve object property with member access value',
                // Bug: { dirPath: rootPath } was erased
                // amaro rejects return statement outside function
                'return { dirPath: rootPath, name: config.name };',
                'return { dirPath: rootPath, name: config.name };', // fibjs: preserves
                null); // amaro: throws

            it('should preserve object property with array value', () => {
                // Bug: { fixIds: [fixId] } was erased to { fixIds         }
                const input = 'const obj = { fixIds: [fixId], items: [] };';
                const expected = 'const obj = { fixIds: [fixId], items: [] };';
                assert.strictEqual(strip(input), expected);
            });

            it('should preserve shorthand arrow function with type keyword as parameter name', () => {
                // Bug: { fn: symbol => {...} } was erased because 'symbol' is a type keyword
                // but here it's used as a parameter name
                const input = '{\n    walkSymbol: symbol => { }\n}';
                const expected = '{\n    walkSymbol: symbol => { }\n}';
                assert.strictEqual(strip(input), expected);
            });

        });

        describe('For Loop Less-Than Operator vs Type Arguments', () => {

            it('should preserve < as comparison operator in for loop', () => {
                // Bug: `i < array.length` was incorrectly parsed as type arguments
                // because `array` looks like a type identifier
                const input = 'for (let i = 0; i < array.length; i++) { console.log(i); }';
                const expected = 'for (let i = 0; i < array.length; i++) { console.log(i); }';
                assert.strictEqual(strip(input), expected);
            });

            it('should preserve < in simple for loop', () => {
                const input = 'for (let i = 0; i < n; i++) {}';
                const expected = 'for (let i = 0; i < n; i++) {}';
                assert.strictEqual(strip(input), expected);
            });

            it('should preserve < in while loop', () => {
                const input = 'while (i < arr.length) { i++; }';
                const expected = 'while (i < arr.length) { i++; }';
                assert.strictEqual(strip(input), expected);
            });

            // amaro throws "Return statement is not allowed here" for this input
            itDiff('should preserve < comparison after identifier',
                'if (value < limit) { return true; }',
                'if (value < limit) { return true; }',  // fibjs: works
                null  // amaro: throws
            );

            it('should preserve < in conditional expression', () => {
                const input = 'const result = a < b ? a : b;';
                const expected = 'const result = a < b ? a : b;';
                assert.strictEqual(strip(input), expected);
            });

            it('should strip actual type arguments in function call', () => {
                // Real type arguments should still be stripped
                const input = 'func<string>(arg);';
                const expected = 'func        (arg);';
                assert.strictEqual(strip(input), expected);
            });

            it('should strip type arguments followed by open paren', () => {
                const input = 'new Map<string, number>();';
                const expected = 'new Map                ();';
                assert.strictEqual(strip(input), expected);
            });

            it('should strip literal type arguments in function call', () => {
                // Type arguments can be literal types like numbers
                const input = 'f11<1>();';
                const expected = 'f11   ();';
                assert.strictEqual(strip(input), expected);
            });

            it('should strip complex literal type arguments', () => {
                const input = 'f<1, 2, 3, 4>();';
                const expected = 'f            ();';
                assert.strictEqual(strip(input), expected);
            });

        });

        describe('Non-null Assertion Position', () => {

            it('should erase non-null assertion without adding extra space', () => {
                // Bug: fibjs preserves expr! while amaro converts to expr + space
                // The space placement differs between implementations
                const input = 'const x = obj!.prop;';
                // fibjs should erase the ! but match amaro behavior
                const expected = 'const x = obj .prop;';
                assert.strictEqual(strip(input), expected);
            });

            it('should erase non-null assertion in function call', () => {
                const input = 'getValue(data!);';
                const expected = 'getValue(data );';
                assert.strictEqual(strip(input), expected);
            });

            it('should erase non-null assertion before method call', () => {
                const input = 'arr!.push(1);';
                const expected = 'arr .push(1);';
                assert.strictEqual(strip(input), expected);
            });

        });

        describe('Ternary Operator in Parentheses', () => {

            it('should preserve ternary operator in parentheses', () => {
                const input = '(a ? b : c)';
                const expected = '(a ? b : c)';
                assert.strictEqual(strip(input), expected);
            });

            it('should preserve ternary operator with property access', () => {
                const input = '(parentNode ? parentNode.end : -1)';
                const expected = '(parentNode ? parentNode.end : -1)';
                assert.strictEqual(strip(input), expected);
            });

            it('should preserve ternary in comparison', () => {
                const input = 'x !== (a ? b : c)';
                const expected = 'x !== (a ? b : c)';
                assert.strictEqual(strip(input), expected);
            });

            it('should preserve ternary with complex expressions', () => {
                const input = '(text ? GeneratedIdentifierFlags.Unique : GeneratedIdentifierFlags.Auto)';
                const expected = '(text ? GeneratedIdentifierFlags.Unique : GeneratedIdentifierFlags.Auto)';
                assert.strictEqual(strip(input), expected);
            });

            it('should preserve ternary with array literal', () => {
                const input = '(referenceRedirect ? [referenceRedirect] : emptyArray)';
                const expected = '(referenceRedirect ? [referenceRedirect] : emptyArray)';
                assert.strictEqual(strip(input), expected);
            });

            it('should still strip optional parameter marker', () => {
                const input = '(a?) => a';
                const expected = '(a ) => a';
                assert.strictEqual(strip(input), expected);
            });

            it('should still strip optional parameter with type', () => {
                const input = '(a?: number) => a';
                const expected = '(a         ) => a';
                assert.strictEqual(strip(input), expected);
            });

            it('should still strip optional parameter in multi-param arrow', () => {
                const input = '(a, b?) => a + b';
                const expected = '(a, b ) => a + b';
                assert.strictEqual(strip(input), expected);
            });

        });

        describe('Contextual Keyword as Variable Name', () => {

            it('should strip type annotation when variable name is symbol keyword', () => {
                const input = 'let symbol: Symbol | undefined;';
                const expected = 'let symbol                    ;';
                assert.strictEqual(strip(input), expected);
            });

            it('should strip type annotation when variable name is type keyword', () => {
                const input = 'let type: string;';
                const expected = 'let type        ;';
                assert.strictEqual(strip(input), expected);
            });

            it('should strip type annotation when variable name is any type keyword', () => {
                const input = 'let any: number = 1;';
                const expected = 'let any         = 1;';
                assert.strictEqual(strip(input), expected);
            });

        });

        describe('Non-null Assertion After Call Expression', () => {

            it('should strip non-null assertion after function call', () => {
                const input = 'func()!';
                const expected = 'func() ';
                assert.strictEqual(strip(input), expected);
            });

            it('should strip non-null assertion after method call', () => {
                const input = 'obj.method()!';
                const expected = 'obj.method() ';
                assert.strictEqual(strip(input), expected);
            });

            it('should strip non-null assertion after call with property access', () => {
                const input = 'func()!.prop';
                const expected = 'func() .prop';
                assert.strictEqual(strip(input), expected);
            });

            it('should strip non-null assertion after chained calls', () => {
                const input = 'arr.get(0)!.value';
                const expected = 'arr.get(0) .value';
                assert.strictEqual(strip(input), expected);
            });

        });

        describe('Union Type with Leading Bar', () => {

            it('should strip type alias with leading bar', () => {
                const input = 'type X = | A | B;';
                const expected = '                 ';
                assert.strictEqual(strip(input), expected);
            });

            it('should strip type alias with leading bar and object types', () => {
                const input = 'type X = | { a: number } | { b: string };';
                const expected = '                                         ';
                assert.strictEqual(strip(input), expected);
            });

            it('should strip multiline union type with leading bar', () => {
                const input = `type Result =
    | { success: true; value: number }
    | { success: false; error: string };`;
                const result = strip(input);
                assert.ok(!result.includes('success'), 'should strip success property');
                assert.ok(!result.includes('value'), 'should strip value property');
                assert.ok(!result.includes('error'), 'should strip error property');
            });

            it('should strip intersection type with leading ampersand', () => {
                const input = 'type X = & A & B;';
                const expected = '                 ';
                assert.strictEqual(strip(input), expected);
            });

        });

        describe('Arrow Function in Ternary Expression', () => {

            it('should preserve ternary else branch after typed arrow function', () => {
                const input = 'x ? ((n: T) => d) : e';
                const expected = 'x ? ((n   ) => d) : e';
                assert.strictEqual(strip(input), expected);
            });

            it('should preserve ternary else branch with complex arrow function', () => {
                const input = 'const x = a ? ((n: T) => b ? c : d) : e;';
                const expected = 'const x = a ? ((n   ) => b ? c : d) : e;';
                assert.strictEqual(strip(input), expected);
            });

            it('should preserve ternary else branch with arrow function returning call', () => {
                const input = 'const f = isExport ? ((node: ModifierLike) => foo(node)) : defaultVisitor;';
                const expected = 'const f = isExport ? ((node              ) => foo(node)) : defaultVisitor;';
                assert.strictEqual(strip(input), expected);
            });

            it('should handle nested parentheses with arrow function', () => {
                const input = 'const x = (((n: T) => n));';
                const expected = 'const x = (((n   ) => n));';
                assert.strictEqual(strip(input), expected);
            });

            it('should preserve ternary else branch after comma expression', () => {
                const input = 'x ? (a = b, c) : d';
                const expected = 'x ? (a = b, c) : d';
                assert.strictEqual(strip(input), expected);
            });

            it('should preserve ternary else branch after complex comma expression', () => {
                const input = 'p.name === "new" ? (newTypeNode = p.type, undefined) : factory.create()';
                const expected = 'p.name === "new" ? (newTypeNode = p.type, undefined) : factory.create()';
                assert.strictEqual(strip(input), expected);
            });

            // Tests for ternary operator vs arrow function return type ambiguity
            // Key insight: `cond ? (a) : v => v` - the `:` is ternary separator, NOT return type
            // But `cond ? (a): T => a : b` - the first `:` is return type, second is ternary separator

            it('should preserve ternary with arrow in false branch (space before colon)', () => {
                // `cond ? (a) : v => v` - `:` is ternary separator, `v => v` is false branch
                const input = 'cond ? (a) : v => v';
                const expected = 'cond ? (a) : v => v';
                assert.strictEqual(strip(input), expected);

            it('should strip arrow return type inside ternary true branch when nested in call', () => {
                const input = 'const r = cond ? foo((x: any): x is number => x > 0) : 0;';
                const expected = 'const r = cond ? foo((x     )              => x > 0) : 0;';
                assert.strictEqual(strip(input), expected);
            });
            });

            it('should preserve ternary with arrow in false branch (no space before colon)', () => {
                // `cond ? (a): v => v` - same as above, just no space
                const input = 'cond ? (a): v => v';
                const expected = 'cond ? (a): v => v';
                assert.strictEqual(strip(input), expected);
            });

            it('should strip return type in standalone arrow function', () => {
                // `(a): v => a` - this is arrow function with return type, should strip `: v`
                const input = '(a): v => a';
                const expected = '(a)    => a';
                assert.strictEqual(strip(input), expected);
            });

            it('should preserve simple ternary without arrow', () => {
                const input = 'x ? (y) : z';
                const expected = 'x ? (y) : z';
                assert.strictEqual(strip(input), expected);
            });

            it('should preserve complex ternary with arrow in false branch', () => {
                // `x ? (a, b) : (c, d) => c + d` - `:` is ternary, `(c, d) => c + d` is false branch
                const input = 'x ? (a, b) : (c, d) => c + d';
                const expected = 'x ? (a, b) : (c, d) => c + d';
                assert.strictEqual(strip(input), expected);
            });

            it('should strip return type in arrow when followed by ternary false branch', () => {
                // `foo ? (bar): baz => qux : other` - `: baz` is return type, `: other` is ternary false
                const input = 'foo ? (bar): baz => qux : other';
                const expected = 'foo ? (bar)      => qux : other';
                assert.strictEqual(strip(input), expected);
            });

            it('should strip return type in nested parens arrow when followed by ternary false branch', () => {
                // `foo ? ((bar): baz => qux) : other` - inner `(bar): baz => qux` is arrow function
                const input = 'foo ? ((bar): baz => qux) : other';
                const expected = 'foo ? ((bar)      => qux) : other';
                assert.strictEqual(strip(input), expected);
            });

            it('should strip return type in arrow with explicit type annotation', () => {
                // `foo ? (a): T => a : b` - `: T` is return type, `: b` is ternary false
                const input = 'foo ? (a): T => a : b';
                const expected = 'foo ? (a)    => a : b';
                assert.strictEqual(strip(input), expected);
            });

            it('should handle deeply nested parentheses in ternary', () => {
                const input = 'foo ? (((bar): baz => qux)) : other';
                const expected = 'foo ? (((bar)      => qux)) : other';
                assert.strictEqual(strip(input), expected);
            });

            it('should handle nested ternary expressions', () => {
                const input = 'a ? b ? c : d : e';
                const expected = 'a ? b ? c : d : e';
                assert.strictEqual(strip(input), expected);
            });

            it('should handle nested ternary in false branch', () => {
                const input = 'a ? b : c ? d : e';
                const expected = 'a ? b : c ? d : e';
                assert.strictEqual(strip(input), expected);
            });

            it('should handle nested ternary with parentheses', () => {
                const input = 'a ? (b ? c : d) : e';
                const expected = 'a ? (b ? c : d) : e';
                assert.strictEqual(strip(input), expected);
            });

            it('should strip return type in nested ternary arrow', () => {
                // Inner ternary `b ? (c): T => c : d` has arrow with return type
                const input = 'a ? b ? (c): T => c : d : e';
                const expected = 'a ? b ? (c)    => c : d : e';
                assert.strictEqual(strip(input), expected);
            });

            it('should handle arrow returning ternary with return type', () => {
                // `(a): T => a ? b : c` is arrow function returning a ternary
                const input = 'x ? (a): T => a ? b : c : d';
                const expected = 'x ? (a)    => a ? b : c : d';
                assert.strictEqual(strip(input), expected);
            });

            it('should handle arrow returning parenthesized ternary', () => {
                const input = 'x ? (a): T => (a ? b : c) : d';
                const expected = 'x ? (a)    => (a ? b : c) : d';
                assert.strictEqual(strip(input), expected);
            });

            it('should handle multi-param arrow in ternary', () => {
                const input = 'cond ? (a, b): T => a + b : fallback';
                const expected = 'cond ? (a, b)    => a + b : fallback';
                assert.strictEqual(strip(input), expected);
            });

            it('should handle typed multi-param arrow in ternary', () => {
                const input = 'cond ? (a: string, b: number): T => a : fallback';
                const expected = 'cond ? (a        , b        )    => a : fallback';
                assert.strictEqual(strip(input), expected);
            });

            it('should preserve comma expression in ternary', () => {
                const input = 'x ? (a = 1, b) : c';
                const expected = 'x ? (a = 1, b) : c';
                assert.strictEqual(strip(input), expected);
            });

            it('should handle arrow with comma expression in false branch', () => {
                const input = 'x ? (a, b): T => a : (c, d)';
                const expected = 'x ? (a, b)    => a : (c, d)';
                assert.strictEqual(strip(input), expected);
            });

            it('should handle generic arrow function in ternary', () => {
                const input = 'x ? <T>(a: T): T => a : fallback';
                const expected = 'x ?    (a   )    => a : fallback';
                assert.strictEqual(strip(input), expected);
            });

            it('should handle method call condition with typed arrow', () => {
                const input = 'obj.method() ? (x): T => x : y';
                const expected = 'obj.method() ? (x)    => x : y';
                assert.strictEqual(strip(input), expected);
            });

            it('should handle array access condition with typed arrow', () => {
                const input = 'arr[0] ? (x): number => x : 0';
                const expected = 'arr[0] ? (x)         => x : 0';
                assert.strictEqual(strip(input), expected);
            });

        });

        describe('Module Keyword as Variable Name', () => {

            it('should preserve module as variable with property access', () => {
                const input = 'module.x = true;';
                const expected = 'module.x = true;';
                assert.strictEqual(strip(input), expected);
            });

            it('should preserve module.exports pattern', () => {
                const input = 'module.exports = foo;';
                const expected = 'module.exports = foo;';
                assert.strictEqual(strip(input), expected);
            });

            it('should preserve module with bracket access', () => {
                const input = 'module["key"] = value;';
                const expected = 'module["key"] = value;';
                assert.strictEqual(strip(input), expected);
            });

            // amaro throws "module keyword is not supported"
            itDiff('should strip declare module declaration',
                'declare module X { }',
                '                    ',  // fibjs: erases declare module
                null  // amaro: throws UnsupportedSyntax
            );

        });

        describe('Template Literal with Regex containing Quote', () => {

            it('should preserve // in template after regex with quote in interpolation', () => {
                // Minimal reproduction: regex /"/ inside ${} followed by }" and // in next template
                const input = '`${/"/}">`\n`// comment`';
                const expected = '`${/"/}">`\n`// comment`';
                assert.strictEqual(strip(input), expected);
            });

            it('should preserve // in template after complex regex interpolation', () => {
                const input = 'const a = `${x.replace(/"/g, "y")}">`;const b = `// comment`;';
                const expected = 'const a = `${x.replace(/"/g, "y")}">`;const b = `// comment`;';
                assert.strictEqual(strip(input), expected);
            });

            it('should preserve // in template after HTML-like template with regex', () => {
                const input = 'const a = `<div data-x="${y.replace(/"/g, \'&quot;\')}"></div>`;\nconst b = `// comment`;';
                const expected = 'const a = `<div data-x="${y.replace(/"/g, \'&quot;\')}"></div>`;\nconst b = `// comment`;';
                assert.strictEqual(strip(input), expected);
            });

        });

        describe('String with Line Continuation (backslash)', () => {

            it('should handle string with CRLF line continuation', () => {
                const input = 'const x = "a;\\\r\n\\\r\nclass A {\\\r\n}";';
                const expected = 'const x = "a;\\\r\n\\\r\nclass A {\\\r\n}";';
                assert.strictEqual(strip(input), expected);
            });

            it('should handle string with LF line continuation', () => {
                const input = 'const x = "a;\\\n\\\nclass A {\\\n}";';
                const expected = 'const x = "a;\\\n\\\nclass A {\\\n}";';
                assert.strictEqual(strip(input), expected);
            });

            it('should handle string with mixed continuation patterns', () => {
                const input = 'const source = "alert(100);\\\r\n\\\r\nclass OverloadedMonster {\\\r\nconstructor();\\\r\nconstructor(name) { }\\\r\n}";';
                const expected = 'const source = "alert(100);\\\r\n\\\r\nclass OverloadedMonster {\\\r\nconstructor();\\\r\nconstructor(name) { }\\\r\n}";';
                assert.strictEqual(strip(input), expected);
            });

        });

        describe('Syntax Error in Import', () => {

            it('should handle syntax error with semicolon in import braces', () => {
                const input = 'import { F1; } from "lib";';
                const expected = 'import { F1; } from "lib";';
                assert.strictEqual(strip(input), expected);
            });

            it('should handle syntax error with class keyword in import', () => {
                const input = 'import { F1, F2 class class class; } from "lib";';
                const expected = 'import { F1, F2 class class class; } from "lib";';
                assert.strictEqual(strip(input), expected);
            });

        });

        describe('Syntax Error in Class Body', () => {

            it('should handle unrecognized token in class body', () => {
                const input = 'class A {\n  x: number\n  ~~\n}';
                // Note: For non-modifier property names (like 'x'), no semicolon is inserted
                // after type erasure, which matches amaro's behavior for valid syntax.
                // amaro would throw an error for this invalid syntax.
                const expected = 'class A {\n  x        \n  ~~\n}';
                assert.strictEqual(strip(input), expected);
            });

        });

        describe('Syntax Error in Export Type', () => {

            it('should handle unrecognized token in export type braces', () => {
                const input = 'export type { ~ };';
                const expected = '                  ';
                assert.strictEqual(strip(input), expected);
            });

            it('should handle semicolon in export type braces', () => {
                const input = 'export type { ; };';
                const expected = '                  ';
                assert.strictEqual(strip(input), expected);
            });

            it('should handle number literal in export type braces', () => {
                const input = 'export type { 123 };';
                const expected = '                    ';
                assert.strictEqual(strip(input), expected);
            });

        });

        describe('Syntax Error in Export Named', () => {

            it('should handle unrecognized token in export braces', () => {
                const input = 'export { ~ };';
                const expected = 'export { ~ };';
                assert.strictEqual(strip(input), expected);
            });

            it('should handle semicolon in export braces', () => {
                const input = 'export { ; };';
                const expected = 'export { ; };';
                assert.strictEqual(strip(input), expected);
            });

            it('should handle number literal in export braces', () => {
                const input = 'export { 123 };';
                const expected = 'export { 123 };';
                assert.strictEqual(strip(input), expected);
            });

        });

        describe('Keyword as Type/Interface Name', () => {
            // In TypeScript, reserved keywords can be used as type alias or interface names
            // This tests that the type stripper correctly handles these cases

            it('should strip type alias with default as name', () => {
                const input = 'type default = string;';
                const expected = '                      ';
                assert.strictEqual(strip(input), expected);
            });

            it('should strip type alias with class as name', () => {
                const input = 'type class = number;';
                const expected = '                    ';
                assert.strictEqual(strip(input), expected);
            });

            it('should strip type alias with function as name', () => {
                const input = 'type function = boolean;';
                const expected = '                        ';
                assert.strictEqual(strip(input), expected);
            });

            it('should strip type alias with abstract as name', () => {
                const input = 'type abstract = string;';
                const expected = '                       ';
                assert.strictEqual(strip(input), expected);
            });

            it('should strip interface with default as name', () => {
                const input = 'interface default { x: number }';
                const expected = '                               ';
                assert.strictEqual(strip(input), expected);
            });

            it('should strip interface with class as name', () => {
                const input = 'interface class { value: string }';
                const expected = '                                 ';
                assert.strictEqual(strip(input), expected);
            });

            it('should strip interface with function as name', () => {
                const input = 'interface function { call(): void }';
                const expected = '                                   ';
                assert.strictEqual(strip(input), expected);
            });

            it('should strip interface with abstract as name', () => {
                const input = 'interface abstract { x: number }';
                const expected = '                                ';
                assert.strictEqual(strip(input), expected);
            });

        });

        describe('Constructor Type with Type Parameters', () => {
            // Constructor types like `new <T>() => T` should be fully stripped

            it('should strip constructor type with type parameters', () => {
                const input = 'var anotherVar: new <T>() => number;';
                const expected = 'var anotherVar                     ;';
                assert.strictEqual(strip(input), expected);
            });

            it('should strip constructor type without type parameters', () => {
                const input = 'var x: new () => number;';
                const expected = 'var x                  ;';
                assert.strictEqual(strip(input), expected);
            });

            it('should strip constructor type with multiple type parameters', () => {
                const input = 'type Ctor = new <T, U extends T>() => U;';
                const expected = '                                        ';
                assert.strictEqual(strip(input), expected);
            });

        });

        describe('Array Type Suffix vs Array Literal on New Line', () => {
            // When [ is on a new line after a type, it should NOT be treated as array type suffix
            // It should be treated as the start of a new statement (array literal)

            it('should not treat array literal on new line as type suffix', () => {
                const input = 'declare const stat: any\n[].push.apply(props);';
                const expected = '                       \n[].push.apply(props);';
                assert.strictEqual(strip(input), expected);
            });

            it('should still strip array type suffix on same line', () => {
                const input = 'declare const arr: number[];';
                const expected = '                            ';
                assert.strictEqual(strip(input), expected);
            });

            it('should handle declare with semicolon followed by array on new line', () => {
                const input = 'declare const props: any[];\n[].push(1);';
                const expected = '                           \n[].push(1);';
                assert.strictEqual(strip(input), expected);
            });

        });

        describe('Object Literal Computed Property Methods', () => {
            // Computed property methods like { [key](params) { } } should have their types stripped

            it('should strip type from computed property method parameter', () => {
                const input = 'const x = { [k](a: number) { } }';
                const expected = 'const x = { [k](a        ) { } }';
                assert.strictEqual(strip(input), expected);
            });

            it('should strip type from generator computed property method', () => {
                const input = 'const x = { *[k](a: number) { } }';
                const expected = 'const x = { *[k](a        ) { } }';
                assert.strictEqual(strip(input), expected);
            });

            it('should strip type from async computed property method', () => {
                const input = 'const x = { async [k](a: number) { } }';
                const expected = 'const x = { async [k](a        ) { } }';
                assert.strictEqual(strip(input), expected);
            });

            it('should strip type from async generator computed property method', () => {
                const input = 'const x = { async *[k](a: number) { } }';
                const expected = 'const x = { async *[k](a        ) { } }';
                assert.strictEqual(strip(input), expected);
            });

            it('should strip type from Symbol.asyncIterator method', () => {
                const input = 'const iter = { async *[Symbol.asyncIterator](_: number) { yield 0; } }';
                const expected = 'const iter = { async *[Symbol.asyncIterator](_        ) { yield 0; } }';
                assert.strictEqual(strip(input), expected);
            });

        });

        describe('Object Method Type Parameters', () => {
            // Object literal method shorthand can have type parameters like { method<T>(param) { } }

            it('should strip type parameters from method shorthand', () => {
                const input = 'const x = { method<T>(param) { } }';
                const expected = 'const x = { method   (param) { } }';
                assert.strictEqual(strip(input), expected);
            });

            it('should strip type parameters from async method', () => {
                const input = 'const x = { async method<T>(param) { } }';
                const expected = 'const x = { async method   (param) { } }';
                assert.strictEqual(strip(input), expected);
            });

            it('should strip type parameters from generator method', () => {
                const input = 'const x = { *gen<T>(param) { } }';
                const expected = 'const x = { *gen   (param) { } }';
                assert.strictEqual(strip(input), expected);
            });

            it('should strip type parameters from computed property method', () => {
                const input = 'const x = { [key]<T>(param) { } }';
                const expected = 'const x = { [key]   (param) { } }';
                assert.strictEqual(strip(input), expected);
            });

            it('should strip type parameters from thenable', () => {
                const input = 'const thenable = { then<V>(onFulfilled) { } }';
                const expected = 'const thenable = { then   (onFulfilled) { } }';
                assert.strictEqual(strip(input), expected);
            });

            it('should strip type parameters with constraints', () => {
                const input = 'const x = { method<T extends string>(param: T) { } }';
                const expected = 'const x = { method                  (param   ) { } }';
                assert.strictEqual(strip(input), expected);
            });

            it('should strip multiple type parameters', () => {
                const input = 'const x = { method<T, U>(a: T, b: U) { } }';
                const expected = 'const x = { method      (a   , b   ) { } }';
                assert.strictEqual(strip(input), expected);
            });

        });

        describe('Export Default Interface/Type', () => {
            // export default interface/type should be completely erased

            it('should strip export default interface entirely', () => {
                const input = 'export default interface zzz { x: string; }';
                const expected = '                                           ';
                assert.strictEqual(strip(input), expected);
            });

            it('should strip export default type entirely', () => {
                const input = 'export default type Foo = string;';
                const expected = '                                 ';
                assert.strictEqual(strip(input), expected);
            });

        });

        describe('Keyword as Property Name in Destructuring', () => {
            // In JavaScript, reserved keywords can be used as property names in object destructuring
            // e.g., `function f({ enum: x }) { return x; }` is valid JS
            // The `enum` here is NOT an enum declaration, it's just a property name

            it('should preserve enum as property name in arrow destructuring', () => {
                const input = '({ enum: x }) => x';
                const expected = '({ enum: x }) => x';
                assert.strictEqual(strip(input), expected);
            });

            it('should preserve enum as property name in function parameter', () => {
                const input = 'function f({ enum: _enum, ...rest }) { return rest; }';
                const expected = 'function f({ enum: _enum, ...rest }) { return rest; }';
                assert.strictEqual(strip(input), expected);
            });

            it('should preserve function as property name in destructuring', () => {
                const input = 'function f({ function: x }) { return x; }';
                const expected = 'function f({ function: x }) { return x; }';
                assert.strictEqual(strip(input), expected);
            });

            it('should preserve class as property name in destructuring', () => {
                const input = 'function f({ class: x }) { return x; }';
                const expected = 'function f({ class: x }) { return x; }';
                assert.strictEqual(strip(input), expected);
            });

            it('should preserve const as property name in destructuring', () => {
                const input = 'function f({ const: x }) { return x; }';
                const expected = 'function f({ const: x }) { return x; }';
                assert.strictEqual(strip(input), expected);
            });

            it('should preserve var as property name in destructuring', () => {
                const input = 'function f({ var: x }) { return x; }';
                const expected = 'function f({ var: x }) { return x; }';
                assert.strictEqual(strip(input), expected);
            });

            it('should preserve if as property name in destructuring', () => {
                const input = 'function f({ if: x }) { return x; }';
                const expected = 'function f({ if: x }) { return x; }';
                assert.strictEqual(strip(input), expected);
            });

            it('should preserve return as property name in destructuring', () => {
                const input = 'function f({ return: x }) { return x; }';
                const expected = 'function f({ return: x }) { return x; }';
                assert.strictEqual(strip(input), expected);
            });

            it('should preserve import as property name in destructuring', () => {
                const input = 'function f({ import: x }) { return x; }';
                const expected = 'function f({ import: x }) { return x; }';
                assert.strictEqual(strip(input), expected);
            });

            it('should preserve export as property name in destructuring', () => {
                const input = 'function f({ export: x }) { return x; }';
                const expected = 'function f({ export: x }) { return x; }';
                assert.strictEqual(strip(input), expected);
            });

            it('should preserve default as property name in destructuring', () => {
                const input = 'function f({ default: x }) { return x; }';
                const expected = 'function f({ default: x }) { return x; }';
                assert.strictEqual(strip(input), expected);
            });

            it('should handle multiple keyword property names', () => {
                const input = 'function f({ enum: e, function: fn, class: c }) { return [e, fn, c]; }';
                const expected = 'function f({ enum: e, function: fn, class: c }) { return [e, fn, c]; }';
                assert.strictEqual(strip(input), expected);
            });

            it('should handle keyword property with type annotation', () => {
                const input = 'function f({ enum: e }: { enum: boolean }) { return e; }';
                const expected = 'function f({ enum: e }                   ) { return e; }';
                assert.strictEqual(strip(input), expected);
            });

            it('should handle keyword property with default value', () => {
                const input = 'function f({ enum: e = true }) { return e; }';
                const expected = 'function f({ enum: e = true }) { return e; }';
                assert.strictEqual(strip(input), expected);
            });

        });

        describe('Generic Arrow Function Edge Cases', () => {

            it('should strip generic with default type parameter', () => {
                const input = '<T = undefined>(value: T): T => value';
                const expected = '               (value   )    => value';
                assert.strictEqual(strip(input), expected);
            });

            it('should strip generic arrow in object literal property', () => {
                const input = '({ test: <T>(v: T) => v })';
                const expected = '({ test:    (v   ) => v })';
                assert.strictEqual(strip(input), expected);
            });

            it('should strip generic with default in object literal', () => {
                const input = '({ test: <T = undefined>(value: T): T => value })';
                const expected = '({ test:                (value   )    => value })';
                assert.strictEqual(strip(input), expected);
            });

            it('should strip generic arrow in nested object return', () => {
                const input = 'const fn = () => ({ test: <T = undefined>(value: T): T => value })';
                const expected = 'const fn = () => ({ test:                (value   )    => value })';
                assert.strictEqual(strip(input), expected);
            });

        });

        describe('Instantiation Expression with Assignment', () => {

            it('should strip type args when instantiation is assigned', () => {
                const input = 'fn<number> = () => 1';
                const expected = 'fn         = () => 1';
                assert.strictEqual(strip(input), expected);
            });

            it('should strip type args on member expression assignment', () => {
                const input = 'obj.fn<T> = x';
                const expected = 'obj.fn    = x';
                assert.strictEqual(strip(input), expected);
            });

            it('should strip type args in complex assignment', () => {
                const input = 'getValue<number> = () => 123';
                const expected = 'getValue         = () => 123';
                assert.strictEqual(strip(input), expected);
            });

        });

        describe('Recursion Depth Limit', () => {

            it('should handle normal nesting depth', () => {
                const depth = 100;
                const input = '('.repeat(depth) + '1' + ')'.repeat(depth);
                const result = strip(input);
                assert.strictEqual(result, input);
            });

            it('should throw error for excessive nesting depth', () => {
                const depth = 600;
                const input = '('.repeat(depth) + '1' + ')'.repeat(depth);
                assert.throws(() => strip(input), /Maximum recursion depth exceeded/);
            });

        });

        describe('Namespace Global', () => {

            it('should strip empty namespace global', () => {
                const input = 'namespace global { }';
                const expected = '                    ';
                assert.strictEqual(strip(input), expected);
            });

            it('should throw error for non-empty namespace global', () => {
                const input = 'namespace global { const x = 1; }';
                assert.throws(() => strip(input), /namespace\/module with body is not supported/);
            });

            it('should strip empty namespace Foo', () => {
                const input = 'namespace Foo { }';
                const expected = '                 ';
                assert.strictEqual(strip(input), expected);
            });

            it('should strip empty module Bar', () => {
                const input = 'module Bar { }';
                const expected = '              ';
                assert.strictEqual(strip(input), expected);
            });

            it('should strip global scope augmentation', () => {
                const input = 'global { interface Array<T> { x } }';
                const expected = '                                   ';
                assert.strictEqual(strip(input), expected);
            });

            it('should strip empty namespace with keyword name', () => {
                const input = 'namespace string { }';
                const expected = ' '.repeat(input.length);
                assert.strictEqual(strip(input), expected);
            });

        });

        describe('Keyword Identifier Edge Cases', () => {

            it('should strip export type with typeof default', () => {
                const input = 'export type X = typeof default;';
                const expected = ' '.repeat(input.length);
                assert.strictEqual(strip(input), expected);
            });

            it('should preserve class named global', () => {
                const input = 'function b() { class global {} }';
                assert.strictEqual(strip(input), input);
            });

        });

        describe('Export Default Function Overloads', () => {

            it('should strip export default function overload signatures', () => {
                const input = 'export default function foo(value: number): number\nexport default function foo(value: string): string\nexport default function foo(value: string | number): string | number {\n    return 1\n}';
                const expected = '                                                  \n                                                  \nexport default function foo(value                 )                  {\n    return 1\n}';
                assert.strictEqual(strip(input), expected);
            });

            it('should strip export default interface after function', () => {
                const input = 'export default function foo() { }\nexport default interface Foo { }';
                const expected = 'export default function foo() { }\n                                ';
                assert.strictEqual(strip(input), expected);
            });

        });

        describe('Arrow Function with Function Type Return', () => {

            it('should strip arrow function with function type as return type', () => {
                const input = 'const fn = <T>(): (() => T) => null as any;';
                const expected = 'const fn =    ()            => null       ;';
                assert.strictEqual(strip(input), expected);
            });

            it('should strip arrow function with complex function type return', () => {
                const input = 'const fn = (): ((a: number) => void) => null;';
                const expected = 'const fn = ()                        => null;';
                assert.strictEqual(strip(input), expected);
            });

            it('should still strip simple function types correctly', () => {
                const input = 'type F = (x: number) => string;';
                const expected = '                               ';
                assert.strictEqual(strip(input), expected);
            });

            it('should strip declare function with complex return type', () => {
                const input = 'declare function fn2(): (cb: () => any) => void;';
                const expected = '                                                ';
                assert.strictEqual(strip(input), expected);
            });

            it('should strip empty parameter function type in variable', () => {
                const input = 'let x: () => T;';
                const expected = 'let x         ;';
                assert.strictEqual(strip(input), expected);
            });

            it('should handle keywords as property names in type references', () => {
                // Keywords like 'default' can be used as property names in type references
                const input = 'declare const x: Q4.default.A;';
                const expected = '                              ';
                assert.strictEqual(strip(input), expected);
            });

            it('should handle import type with member access and type arguments', () => {
                // import("module").Member<...> with << token
                const input = 'export declare const x: import("m").Modifier<<T>(x: T) => T>;';
                const expected = '                                                             ';
                assert.strictEqual(strip(input), expected);
            });

            it('should remove non-null assertions in decorators', () => {
                // Non-null assertions (!) are type-level and should be removed from decorators
                const input1 = '{ @x! class C {} }';
                const expected1 = '{ @x  class C {} }';
                assert.strictEqual(strip(input1), expected1);
                
                const input2 = '{ @x.y! class C {} }';
                const expected2 = '{ @x.y  class C {} }';
                assert.strictEqual(strip(input2), expected2);
                
                const input3 = '{ @x!.y class C {} }';
                const expected3 = '{ @x .y class C {} }';
                assert.strictEqual(strip(input3), expected3);
            });

            it('should handle interleaved non-null and member access in decorators', () => {
                // Test that @x!.y followed by @g<number> works correctly
                const input = '{ @x!.y class C {} }\n\n{ @g<number>() class C {} }';
                const expected = '{ @x .y class C {} }\n\n{ @g        () class C {} }';
                assert.strictEqual(strip(input), expected);
            });

            it('should remove type arguments in decorators', () => {
                // Type arguments in decorators should be removed
                const input1 = '{ @g<number>() class C {} }';
                const expected1 = '{ @g        () class C {} }';
                assert.strictEqual(strip(input1), expected1);
                
                // Also in parenthesized decorator expressions
                const input2 = '{ @(g<number>) class C {} }';
                const expected2 = '{ @(g        ) class C {} }';
                assert.strictEqual(strip(input2), expected2);
            });

            it('should handle typeof import type correctly', () => {
                // typeof import(...) should be completely erased in type positions
                const input1 = "let x: typeof import('./a');";
                const expected1 = "let x                      ;";
                assert.strictEqual(strip(input1), expected1);
                
                // With member access
                const input2 = "type T1 = typeof import('./input.js').myFunction;";
                const expected2 = "                                                 ";
                assert.strictEqual(strip(input2), expected2);
                
                // With type arguments (instantiation expression)
                const input3 = "type T2 = typeof import('./input.js').myFunction<any>;";
                const expected3 = "                                                      ";
                assert.strictEqual(strip(input3), expected3);
            });

            it('should handle import type with from as import name', () => {
                // import type from from '...' - 'type' is modifier, first 'from' is import name
                const input1 = "import type from from './a';";
                const expected1 = "                            ";
                assert.strictEqual(strip(input1), expected1);
                
                // import type from = require('..') - 'type' is modifier, 'from' is alias
                const input2 = "import type from = require('./a');";
                const expected2 = "                                  ";
                assert.strictEqual(strip(input2), expected2);
                
                // import type from '...' - 'type' is default import name (not erased)
                const input3 = "import type from './a';";
                const expected3 = "import type from './a';";
                assert.strictEqual(strip(input3), expected3);
            });

            it('should handle import type with defer modifier', () => {
                // import type defer * as ns from '...' - should be completely erased
                const input1 = 'import type defer * as ns from "./a";';
                const expected1 = '                                     ';
                assert.strictEqual(strip(input1), expected1);
            });

            it('should handle decorated class expression in function parameter', () => {
                // Decorated class expression as default parameter value
                const input = 'function f(C = @dec class {}) {}';
                const expected = 'function f(C = @dec class {}) {}';
                assert.strictEqual(strip(input), expected);
            });

            it('should not treat binary operators as type arguments', () => {
                // a < b || b > should not be parsed as generic type arguments
                const input1 = 'if (a < b || b > (c + 1)) { }';
                const expected1 = 'if (a < b || b > (c + 1)) { }';
                assert.strictEqual(strip(input1), expected1);

                // a < b && b > should not be parsed as generic type arguments  
                const input2 = 'if (a < b && b > (c + 1)) { }';
                const expected2 = 'if (a < b && b > (c + 1)) { }';
                assert.strictEqual(strip(input2), expected2);

                // Mixed with actual generics
                const input3 = 'const x = a < b || foo<T>();';
                const expected3 = 'const x = a < b || foo   ();';
                assert.strictEqual(strip(input3), expected3);
            });

            it('should handle module keyword as variable with in operator', () => {
                // 'module in {}' is valid JS expression, not TypeScript module declaration
                const input1 = 'let module = 10;\nmodule in {}';
                const expected1 = 'let module = 10;\nmodule in {}';
                assert.strictEqual(strip(input1), expected1);

                // 'module in' should not be confused with module declaration
                const input2 = 'if (module in obj) { }';
                const expected2 = 'if (module in obj) { }';
                assert.strictEqual(strip(input2), expected2);

                // 'namespace in' should also work
                const input3 = 'let namespace = {};\nnamespace in obj';
                const expected3 = 'let namespace = {};\nnamespace in obj';
                assert.strictEqual(strip(input3), expected3);
            });

            it('should handle >= token after type arguments in variable declaration', () => {
                // When there's no space between > and =, the lexer produces >= token
                // The parser must rescan >= as > followed by = to correctly parse type annotation
                const input1 = 'var v : Foo<T>= 1;';
                const expected1 = 'var v         = 1;';
                assert.strictEqual(strip(input1), expected1);

                // With space - normal case for comparison
                const input2 = 'var v : Foo<T> = 1;';
                const expected2 = 'var v          = 1;';
                assert.strictEqual(strip(input2), expected2);

                // Multiple type parameters
                const input3 = 'let x: Map<string, number>= new Map();';
                const expected3 = 'let x                     = new Map();';
                assert.strictEqual(strip(input3), expected3);
            });

            it('should handle unicode escape sequences in property names with type annotations', () => {
                // \\uXXXX format
                const input1 = 'class C { \\u0078: number; }';
                const expected1 = 'class C { \\u0078        ; }';
                assert.strictEqual(strip(input1), expected1);

                // \\u{XXXX} format (extended unicode escape)
                const input2 = 'class C { \\u{78}: number; }';
                const expected2 = 'class C { \\u{78}        ; }';
                assert.strictEqual(strip(input2), expected2);

                // Mixed with regular identifier
                const input3 = 'class C { x\\u{78}y: string; }';
                const expected3 = 'class C { x\\u{78}y        ; }';
                assert.strictEqual(strip(input3), expected3);
            });

            it('should preserve regex patterns that look like comments', () => {
                // Regex with /* */ pattern
                const input1 = 'const ML_COMMENT = /\\/\\*[\\s\\S]*?\\*\\//;';
                const expected1 = 'const ML_COMMENT = /\\/\\*[\\s\\S]*?\\*\\//;';
                assert.strictEqual(strip(input1), expected1);

                // Regex with // pattern
                const input2 = 'const SL_COMMENT = /\\/\\//;';
                const expected2 = 'const SL_COMMENT = /\\/\\//;';
                assert.strictEqual(strip(input2), expected2);

                // Combined in object
                const input3 = `const obj = {
    ML_COMMENT: /\\/\\*[\\s\\S]*?\\*\\//,
    SL_COMMENT: /\\/\\/[^\\n\\r]*/,
};`;
                const expected3 = `const obj = {
    ML_COMMENT: /\\/\\*[\\s\\S]*?\\*\\//,
    SL_COMMENT: /\\/\\/[^\\n\\r]*/,
};`;
                assert.strictEqual(strip(input3), expected3);
            });

            it('should preserve abstract and override as property names', () => {
                // abstract as property name
                const input1 = 'class C { abstract = false; }';
                const expected1 = 'class C { abstract = false; }';
                assert.strictEqual(strip(input1), expected1);

                // override as property name with type
                const input2 = 'class C { override: string; }';
                const expected2 = 'class C { override        ; }';
                assert.strictEqual(strip(input2), expected2);

                // Both as modifiers (should be removed)
                const input3 = 'class C { override foo(): void {} abstract bar(): void; }';
                const expected3 = 'class C {          foo()       {}                       }';
                assert.strictEqual(strip(input3), expected3);

                // Mixed usage
                const input4 = 'class C { abstract = false; override foo() {} }';
                const expected4 = 'class C { abstract = false;          foo() {} }';
                assert.strictEqual(strip(input4), expected4);
            });

            it('should strip template literal types in type arguments', () => {
                // Simple template literal type
                const input1 = 'test<`hello`>();';
                const expected1 = 'test         ();';
                assert.strictEqual(strip(input1), expected1);

                // Empty template literal type
                const input2 = 'test<``>();';
                const expected2 = 'test    ();';
                assert.strictEqual(strip(input2), expected2);

                // Template literal type with expression
                const input3 = 'test<`hello ${x as number} world`>();';
                const expected3 = 'test                              ();';
                assert.strictEqual(strip(input3), expected3);

                // Nested type arguments with template literal
                const input4 = 'expectTypeOf<z.infer<typeof empty>>().toEqualTypeOf<``>();';
                const expected4 = 'expectTypeOf                       ().toEqualTypeOf    ();';
                assert.strictEqual(strip(input4), expected4);

                // Union type with template literals (leading |)
                const input5 = 'test< | `${number}` | `${number}px`>();';
                const expected5 = 'test                                ();';
                assert.strictEqual(strip(input5), expected5);

                // Intersection type (leading &)
                const input6 = 'test< & { a: string } & { b: number }>();';
                const expected6 = 'test                                  ();';
                assert.strictEqual(strip(input6), expected6);
            });

            it('should strip complex nested template literal types', () => {
                // Nested template strings
                const input1 = 'test<`outer ${`inner ${x as number}`} end`>();';
                const expected1 = 'test                                       ();';
                assert.strictEqual(strip(input1), expected1);

                // Multiple embedded expressions
                const input2 = 'test<`a ${x as string} b ${y as number} c`>();';
                const expected2 = 'test                                       ();';
                assert.strictEqual(strip(input2), expected2);

                // Embedded object literals
                const input3 = 'test<`value: ${obj as { a: number }}`>();';
                const expected3 = 'test                                  ();';
                assert.strictEqual(strip(input3), expected3);

                // Embedded function calls with type arguments
                const input4 = 'test<`result: ${fn<T>() as string}`>();';
                const expected4 = 'test                                ();';
                assert.strictEqual(strip(input4), expected4);

                // Embedded ternary expressions
                const input5 = 'test<`${x ? (a as string) : (b as number)}`>();';
                const expected5 = 'test                                        ();';
                assert.strictEqual(strip(input5), expected5);

                // Embedded arrays with generics
                const input6 = 'test<`items: ${arr as Array<string>}`>();';
                const expected6 = 'test                                  ();';
                assert.strictEqual(strip(input6), expected6);
            });

            it('should preserve template string values while stripping embedded types', () => {
                // Pure template string with embedded type arguments
                const input1 = 'console.log(`result: ${fn<T>() as string}`);';
                const expected1 = 'console.log(`result: ${fn   ()          }`);';
                assert.strictEqual(strip(input1), expected1);

                // Multiple expressions with types
                const input2 = 'const msg = `a: ${x as number}, b: ${y as string}`;';
                const expected2 = 'const msg = `a: ${x          }, b: ${y          }`;';
                assert.strictEqual(strip(input2), expected2);

                // Nested template strings with types
                const input3 = 'const nested = `outer ${`inner ${getValue<T>()}`}`;';
                const expected3 = 'const nested = `outer ${`inner ${getValue   ()}`}`;';
                assert.strictEqual(strip(input3), expected3);

                // Template with conditional and types
                const input4 = 'const cond = `value: ${x ? (a as string) : (b as number)}`;';
                const expected4 = 'const cond = `value: ${x ? (a          ) : (b          )}`;';
                assert.strictEqual(strip(input4), expected4);

                // Template with object literal types
                const input5 = 'const obj = `data: ${o as { x: number }}`;';
                const expected5 = 'const obj = `data: ${o                 }`;';
                assert.strictEqual(strip(input5), expected5);

                // Template with array types
                const input6 = 'const arr = `items: ${list as Array<string>}`;';
                const expected6 = 'const arr = `items: ${list                 }`;';
                assert.strictEqual(strip(input6), expected6);

                // Complex expression with multiple type operations
                const input7 = 'const complex = `result: ${fn<T, U>(x as T) + (y as U)}`;';
                const expected7 = 'const complex = `result: ${fn      (x     ) + (y     )}`;';
                assert.strictEqual(strip(input7), expected7);
            });

            it('should handle deeply nested template strings with types', () => {
                // Multiple template strings in one expression (no types)
                const input1 = 'console.log(`result: ${`string1 ${exp1}` + `string2 ${exp2}`}`);';
                const expected1 = 'console.log(`result: ${`string1 ${exp1}` + `string2 ${exp2}`}`);';
                assert.strictEqual(strip(input1), expected1);

                // Multiple templates with type annotations
                const input2 = 'console.log(`result: ${`string1 ${exp1 as number}` + `string2 ${exp2 as string}`}`);';
                const expected2 = 'console.log(`result: ${`string1 ${exp1          }` + `string2 ${exp2          }`}`);';
                assert.strictEqual(strip(input2), expected2);

                // Triple nesting with types
                const input3 = 'const msg = `outer: ${`mid1: ${`inner1: ${x as number}`}` + `mid2: ${`inner2: ${y as string}`}`}`);';
                const expected3 = 'const msg = `outer: ${`mid1: ${`inner1: ${x          }`}` + `mid2: ${`inner2: ${y          }`}`}`);';
                assert.strictEqual(strip(input3), expected3);

                // Multiple operations with templates and generic types
                const input4 = 'const complex = `value: ${`a ${x as T}` + `b ${y as U}` + `c ${z as V}`}`;';
                const expected4 = 'const complex = `value: ${`a ${x     }` + `b ${y     }` + `c ${z     }`}`;';
                assert.strictEqual(strip(input4), expected4);

                // Function calls with template arguments
                const input5 = 'const fn = `result: ${foo(`arg1 ${a as number}`) + bar(`arg2 ${b as string}`)}`);';
                const expected5 = 'const fn = `result: ${foo(`arg1 ${a          }`) + bar(`arg2 ${b          }`)}`);';
                assert.strictEqual(strip(input5), expected5);

                // Nested ternary with templates
                const input6 = 'const tern = `outer: ${x ? `true: ${a as string}` : `false: ${b as number}`}`;';
                const expected6 = 'const tern = `outer: ${x ? `true: ${a          }` : `false: ${b          }`}`;';
                assert.strictEqual(strip(input6), expected6);

                // Array of templates with types
                const input7 = 'const arr = `items: ${[`first ${x as number}`, `second ${y as string}`].join(\', \')}`;';
                const expected7 = 'const arr = `items: ${[`first ${x          }`, `second ${y          }`].join(\', \')}`;';
                assert.strictEqual(strip(input7), expected7);
            });

        });

    });

});
