# SequentProver

A simple implementation of a theorem prover using sequent calculus with some supplied rules (as part of an assignment for a knowledge representation and reasoning course at UNSW).

## Basics

Below, ![](http://mathurl.com/render.cgi?%5Cpi), ![](http://mathurl.com/render.cgi?%5Crho), ![](http://mathurl.com/render.cgi?%5Clambda), and ![](http://mathurl.com/render.cgi?%5Czeta) are (possibly empty) strings of formulae, and ![](http://mathurl.com/render.cgi?%5Cphi) and ![](http://mathurl.com/render.cgi?%5Cpsi) are non-empty formulae. The following 11 rules are those which the prover uses to construct its proof:
<table>
  <tr>
    <td><img src="http://mathurl.com/render.cgi?%5Cbegin%7Barray%7D%7Bc%7D%5Csquare%20%5C%5C%5Chline%20%5Cpi%2C%20%5Cphi%2C%20%5Crho%20%5Cvdash%20%5Clambda%2C%20%5Cphi%2C%20%5Czeta%5Cend%7Barray%7D%20%5C%3B%20%28P1%29" /></td>
    <td></td>
  </tr>
  <tr>
    <td><img src="http://mathurl.com/render.cgi?%5Cbegin%7Barray%7D%7Bc%7D%5Cpi%2C%20%5Cphi%2C%20%5Crho%20%5Cvdash%20%5Clambda%2C%20%5Czeta%20%5C%5C%5Chline%20%5Cpi%2C%20%5Crho%20%5Cvdash%20%5Clambda%2C%20%5Cneg%20%5Cphi%2C%20%5Czeta%5Cend%7Barray%7D%20%5C%3B%20%28P2a%29" /></td>
    <td><img src="http://mathurl.com/render.cgi?%5Cbegin%7Barray%7D%7Bc%7D%5Cpi%2C%20%5Crho%20%5Cvdash%20%5Clambda%2C%20%5Cphi%2C%20%5Czeta%20%5C%5C%5Chline%20%5Cpi%2C%20%5Cneg%20%5Cphi%2C%20%5Crho%20%5Cvdash%20%5Clambda%2C%20%5Czeta%5Cend%7Barray%7D%20%5C%3B%20%28P2b%29" /></td>
  </tr>
  <tr>
    <td><img src="http://mathurl.com/render.cgi?%5Cbegin%7Barray%7D%7Bc%7D%5Cpi%20%5Cvdash%20%5Crho%2C%20%5Cphi%2C%20%5Clambda%3B%20%5C%3B%20%5Cpi%20%5Cvdash%20%5Crho%2C%20%5Cpsi%2C%20%5Clambda%20%5C%5C%5Chline%20%5Cpi%20%5Cvdash%20%5Crho%2C%20%5Cphi%20%5Cwedge%20%5Cpsi%2C%20%5Clambda%5Cend%7Barray%7D%20%5C%3B%20%28P3a%29" /></td>
    <td><img src="http://mathurl.com/render.cgi?%5Cbegin%7Barray%7D%7Bc%7D%5Cpi%2C%20%5Cphi%2C%20%5Cpsi%2C%20%5Crho%20%5Cvdash%20%5Clambda%20%5C%5C%5Chline%20%5Cpi%2C%20%5Cphi%20%5Cwedge%20%5Cpsi%2C%20%5Crho%20%5Cvdash%20%5Clambda%5Cend%7Barray%7D%20%5C%3B%20%28P3b%29" /></td>
  </tr>
  <tr>
    <td><img src="http://mathurl.com/render.cgi?%5Cbegin%7Barray%7D%7Bc%7D%5Cpi%20%5Cvdash%20%5Crho%2C%20%5Cphi%2C%20%5Cpsi%2C%20%5Clambda%20%5C%5C%5Chline%20%5Cpi%20%5Cvdash%20%5Crho%2C%20%5Cphi%20%5Cvee%20%5Cpsi%2C%20%5Clambda%5Cend%7Barray%7D%20%5C%3B%20%28P4a%29" /></td>
    <td><img src="http://mathurl.com/render.cgi?%5Cbegin%7Barray%7D%7Bc%7D%5Cpi%2C%20%5Cphi%2C%20%5Crho%20%5Cvdash%20%5Clambda%3B%20%5C%3B%20%5Cpi%2C%20%5Cpsi%2C%20%5Crho%20%5Cvdash%20%5Clambda%20%5C%5C%5Chline%20%5Cpi%2C%20%5Cphi%20%5Cvee%20%5Cpsi%2C%20%5Crho%20%5Cvdash%20%5Clambda%5Cend%7Barray%7D%20%5C%3B%20%28P4b%29" /></td>
  </tr>
  <tr>
    <td><img src="http://mathurl.com/render.cgi?%5Cbegin%7Barray%7D%7Bc%7D%5Cpi%2C%20%5Cphi%2C%20%5Crho%20%5Cvdash%20%5Clambda%2C%20%5Cpsi%2C%20%5Czeta%20%5C%5C%5Chline%20%5Cpi%2C%20%5Crho%20%5Cvdash%20%5Clambda%2C%20%5Cphi%20%5Cto%20%5Cpsi%2C%20%5Czeta%5Cend%7Barray%7D%20%5C%3B%20%28P5a%29" /></td>
    <td><img src="http://mathurl.com/render.cgi?%5Cbegin%7Barray%7D%7Bc%7D%5Cpi%2C%20%5Cphi%2C%20%5Crho%20%5Cvdash%20%5Clambda%2C%20%5Czeta%3B%20%5C%3B%20%5Cpi%2C%20%5Crho%20%5Cvdash%20%5Clambda%2C%20%5Cpsi%2C%20%5Czeta%20%5C%5C%5Chline%20%5Cpi%2C%20%5Cpsi%20%5Cto%20%5Cphi%2C%20%5Crho%20%5Cvdash%20%5Clambda%2C%20%5Czeta%5Cend%7Barray%7D%20%5C%3B%20%28P5b%29" /></td>
  </tr>
  <tr>
    <td><img src="http://mathurl.com/render.cgi?%5Cbegin%7Barray%7D%7Bc%7D%5Cpi%2C%20%5Cphi%2C%20%5Crho%20%5Cvdash%20%5Clambda%2C%20%5Cpsi%2C%20%5Czeta%3B%20%5C%3B%20%5Cpi%2C%20%5Cpsi%2C%20%5Crho%20%5Cvdash%20%5Clambda%2C%20%5Cphi%2C%20%5Czeta%20%5C%5C%5Chline%20%5Cpi%2C%20%5Crho%20%5Cvdash%20%5Clambda%2C%20%5Cphi%20%5Cleftrightarrow%20%5Cpsi%2C%20%5Czeta%5Cend%7Barray%7D%20%5C%3B%20%28P6a%29" /></td>
    <td><img src="http://mathurl.com/render.cgi?%5Cbegin%7Barray%7D%7Bc%7D%5Cpi%2C%20%5Cphi%2C%20%5Cpsi%2C%20%5Crho%20%5Cvdash%20%5Clambda%2C%20%5Czeta%3B%20%5C%3B%20%5Cpi%2C%20%5Crho%20%5Cvdash%20%5Clambda%2C%20%5Cphi%2C%20%5Cpsi%2C%20%5Czeta%20%5C%5C%5Chline%20%5Cpi%2C%20%5Cphi%20%5Cleftrightarrow%20%5Cpsi%2C%20%5Crho%20%5Cvdash%20%5Clambda%2C%20%5Czeta%5Cend%7Barray%7D%20%5C%3B%20%28P6b%29" /></td>
  </tr>
</table>

To do so, it searches backwards, starting with the sequent to prove. For example, one possible proof that ![](http://mathurl.com/render.cgi?%5Cpsi%20%5Cto%20%5Cphi%20%5Cvdash%20%5Cphi%20%5Cvee%20%5Cneg%20%5Cpsi) is

![](http://mathurl.com/render.cgi?%5Cbegin%7Barray%7D%7Bc@%7B%7E%7Dl@%7B%5Cquad%7Dl%7D%5Cphi%2C%20%5Cpsi%20%5Cvdash%20%5Cphi%3B%20%5C%3B%20%5Cpsi%20%5Cvdash%20%5Cphi%2C%20%5Cpsi%20%26%26%20%28P1%3B%20P1%29%5C%5C%5Ccline%7B1-1%7D%20%5Cpsi%20%5Cto%20%5Cphi%2C%20%5Cpsi%20%5Cvdash%20%5Cphi%20%26%26%20%28P5b%29%20%5C%5C%5Ccline%7B1-1%7D%20%5Cpsi%20%5Cto%20%5Cphi%20%5Cvdash%20%5Cphi%2C%20%5Cneg%20%5Cpsi%20%26%26%20%28P2a%29%20%5C%5C%5Ccline%7B1-1%7D%20%5Cpsi%20%5Cto%20%5Cphi%20%5Cvdash%20%5Cphi%20%5Cvee%20%5Cneg%20%5Cpsi%20%26%26%20%28P4a%29%20%5Cend%7Barray%7D)

Instead of searching forward for a proof, we can begin at ![](http://mathurl.com/render.cgi?%5Cpsi%20%5Cto%20%5Cphi%20%5Cvdash%20%5Cphi%20%5Cvee%20%5Cneg%20%5Cpsi), and apply rules P4a, P2a, and P5b backwards, where P5b splits up ![](http://mathurl.com/render.cgi?%5Cpsi%20%5Cto%20%5Cphi%2C%20%5Cpsi%20%5Cvdash%20%5Cphi) into two sequents, ![](http://mathurl.com/render.cgi?%5Cphi%2C%20%5Cpsi%20%5Cvdash%20%5Cphi) and ![](http://mathurl.com/render.cgi?%5Cpsi%20%5Cvdash%20%5Cphi%2C%20%5Cpsi), which can both be proven by P1.

## Usage

Running `make` will produce the binary `assn1q3`.

### Input

The program is run as `./assn1q3 <sequent>`, where `<sequent>` is of the form `[formulae] seq [formulae]`. The `formulae` consist of comma-separated formulae using the following representations:

| Symbol | Representation | Description |
| :--- | :--- | :--- |
| ![](http://mathurl.com/render.cgi?%5Cneg) | `neg` | Negation |
| ![](http://mathurl.com/render.cgi?%5Cvee) | `or` | Disjunction |
| ![](http://mathurl.com/render.cgi?%5Cwedge) | `and` | Conjunction |
| ![](http://mathurl.com/render.cgi?%5Cto) | `imp` | Implication |
| ![](http://mathurl.com/render.cgi?%5Cleftrightarrow) | `iff` | Bi-implication |

Any other input is treated as an atom (for example, `hello`), which is case-sensitive. Grouping is by parentheses `()`, and order of precedence is right associative, in the order:

1. `neg`,
2. `or` and `and`,
3. `imp` and `iff`,

So, `neg p or q and r or s imp t` is parsed as `((neg p) or (q and (r or s))) imp t`.

The sequent ![](http://mathurl.com/render.cgi?%5Cphi%2C%20%28%5Cphi%20%5Cto%20%5Cpsi%29%20%5Cwedge%20%5Crho%20%5Cvdash%20%5Cpsi%20%5Cvee%20%5Cneg%20%5Crho) could be input as `[phi, (phi imp psi) and rho] seq [psi or neg rho]`.

Theorems can be inputted by leaving the left side of the sequent empty, so `[] seq [p or neg p]`, for example.

### Output

The first line of output (assuming valid input) will be either `true` or `false`, indicating whether a proof of the sequent was found. If it is `true`, then the proof found will be output on the following lines. For example:
```sh
$ ./assn1q3 `[p imp q, q] seq [p]`
false
$ ./assn1q3 `[p imp q] seq [q or neg p]`
true
1: [q] seq [q, neg p] ... via P1
2: [q] seq [q or (neg p)] ... via P4a from 1
3: [p] seq [q, p] ... via P1
4: [] seq [q, neg p, p] ... via P2a from 3
5: [] seq [q or (neg p), p] ... via P4a from 4
6: [p imp q] seq [q or (neg p)] ... via P5b from 2, 5  
```
