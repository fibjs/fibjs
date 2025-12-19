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

    });

    describe('Instantiation Expression', () => {

        // Instantiation expression without semicolon should still strip the generic
        it('should handle instantiation expression without semicolon', () => {
            assert.strictEqual(strip('const foo = call<string>'), 'const foo = call        ');
        });

        it('should strip instantiation expression with semicolon', () => {
            assert.strictEqual(strip('const fn = genericFn<string, number>;'), 'const fn = genericFn                ;');
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
        // Semicolon is needed after property named "static" to prevent ASI issues

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
                const expected = 'class A {\n  x;       \n  ~~\n}';
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

    });

});
