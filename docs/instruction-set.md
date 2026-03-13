# Sulfur CPU Instruction Set

このドキュメントは、現在の実装に合わせた命令セット仕様です。

## 1. 基本仕様

- 命令長: 16ビット固定長
- メモリアドレス: ワード単位
- レジスタ: R0-R7（R6=LR, R7=SP）
- プログラムカウンタ: PC

## 2. 命令フォーマット

### R型

- ビット配置: [opcode:4][rd:3][rs1:3][rs2:3][func:3]
- 用途: レジスタ間ALU演算

### I6型

- ビット配置: [opcode:4][rd:3][rs:3][imm:6]
- 用途: load/store
- 即値: 6ビット符号付き

### I9型

- ビット配置: [opcode:4][rd:3][imm:9]
- 用途: addi/subi, 条件分岐
- 即値: 9ビット符号付き

### J型

- ビット配置: [opcode:4][imm:12]
- 用途: jal
- 即値: 12ビット符号付き

### SP型

- ビット配置: [opcode:4][0:12]
- 用途: ret, halt などの特殊命令

## 3. Opcode / Func 一覧

### 3.1 R型（opcode=0b0000）

| mnemonic | func(3bit) | 動作                         |
| -------- | ---------: | ---------------------------- |
| add      |        000 | rd = rs1 + rs2               |
| sub      |        001 | rd = rs1 - rs2               |
| slt      |        010 | rd = (int16)rs1 < (int16)rs2 |
| and      |        011 | rd = rs1 & rs2               |
| or       |        100 | rd = rs1 \| rs2              |
| xor      |        101 | rd = rs1 ^ rs2               |
| shl      |        110 | rd = rs1 << (rs2 & 0xF)      |
| shr      |        111 | rd = rs1 >> (rs2 & 0xF)      |

### 3.2 非R型

| mnemonic | opcode(4bit) | 型  | アセンブリ表記   | 動作                    |
| -------- | -----------: | --- | ---------------- | ----------------------- |
| addi     |         0001 | I9  | addi $rd, imm    | rd = rd + imm           |
| subi     |         0010 | I9  | subi $rd, imm    | rd = rd - imm           |
| ld       |         0011 | I6  | ld $rd, $rs, off | rd = MEM[rs + off]      |
| sr       |         0100 | I6  | sr $rd, $rs, off | MEM[rs + off] = rd      |
| beqz     |         0101 | I9  | beqz $rd, label  | if rd==0 then PC += off |
| bnez     |         0110 | I9  | bnez $rd, label  | if rd!=0 then PC += off |
| jal      |         0111 | J   | jal label/imm    | R6=PC+1, PC += off      |
| ret      |         1000 | SP  | ret              | PC = R6                 |
| halt     |         1111 | SP  | halt             | 実行停止                |

## 4. 即値エンコード規則

アセンブラは命令生成時に、型ごとのビット幅で即値をマスクして格納します。

- I6: imm & 0x3F
- I9: imm & 0x1FF
- J : imm & 0xFFF

CPU側デコード時に符号拡張します。

- I6: sign_extend(imm, 6)
- I9: sign_extend(imm, 9)
- J : sign_extend(imm, 12)

## 5. 分岐オフセット規則

ラベル参照時の相対オフセットは次式。

offset = target_address - (current_address + 1)

このため、1命令先へ飛ぶ場合は offset=1、1命令前へ戻る場合は offset=-1。

## 6. 実行上の注意

- R0は各命令実行後に0へ固定
- store命令でROM領域（0x0000-0x1FFF）への書き込みは例外扱い
- 命令/データメモリは同一空間
