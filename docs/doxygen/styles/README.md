Doxygen の LaTeX 出力で日本語を使う（LaTeX スタイルシートによって）
===============================================================

- 参考 URL:
    - [イグトランスの頭の中（のかけら）: 2011年版](https://dev.activebasic.com/egtra/2011/12/25/459/)
    - [イグトランスの頭の中（のかけら）: 2015年版](https://dev.activebasic.com/egtra/2015/06/29/814/)
    - [LaTeX の「アレなデフォルト」傾向と対策](https://qiita.com/zr_tex8r/items/297154ca924749e62471)
    - [TeX Wiki / hyperref](https://texwiki.texjp.org/?hyperref)
    - [Doxygenの設定メモ - MindTo01s](http//www.mindto01s.com/2017/04/11/023e9cbe_c712_48d3_9667_0d381a06cfb1.html)

概要
----

参考 URL の最初の2つのサイトにあるように、Doxygen の LaTeX 出力ソースからの PDF 作成は、
欧文の LaTeX もしくは pdfLaTeX を想定しており、そのままでは日本語が使えません。
PDF の日本語が文字化けしてしまいます。

そこで上記参考 URL のサイトを参考にして、それらのサイトでの修正と同様ことを行う
LaTeX スタイルシートを作ってみました。

参考サイトでは、Doxygen が出力した LaTeX ソースファイル（`refman.tex`）を
`sed` もしくは手動で編集しています。
Doxygen を実行するたびに修正するのは、ほんの僅かではありますが手間がかかります。

以降で説明するいくつかのスタイルシートでは、それらのファイル名を
Doxygen の `Doxyfile` に書いておくだけで、
出力された `refman.tex` を修正することなしに、
日本語の PDF を文字化けすることなしに生成することができるようになります。

また、私の環境だけかもしれませんが、 Doxygen の出力から作成した PDF では、
関数名などのリンクをクリックしたときに、その定義場所にジャンプできていませんでした。
その現象の修正も LaTeX スタイルシートで行っています。

- - - - - - - - -

動作環境
-------

動作確認は以下の環境で行いましたが、他の環境でも同じスタイルシートが使えると期待しています。

- オペレーティングシステム：
    - Windows 7 Professional 64bit
    - macOS High Sierra
- アプリケーション：
    - Doxygen: 1.8.14 (Cygwin 版 / Homebrew 版)
    - LaTeX: e-upTeX 3.14159265-p3.8.1-u1.23-180226-2.6 (utf8.uptex) (TeX Live 2018)

- - - - - - - - -

Doxygen の設定
--------------

プロジェクト用の Doxyfile に、以下の設定を行います。

### GENERATE_LATEX

    GENERATE_LATEX = YES

LaTeX 出力を有効にします。

### LATEX_CMD_NAME

    LATEX_CMD_NAME = uplatex

LaTeX のコマンド名に、日本語に対応した upLaTeX のコマンドを指定します。

### USE_PDFLATEX

    USE_PDFLATEX = NO

pdfLaTeX は日本語を扱えないため、これを使わないようにします。

### LATEX_EXTRA_STYLESHEET

    LATEX_EXTRA_STYLESHEET = doxygenjpext.sty pdfinfo.sty fontsettings.sty hyperlinkfix.sty

以降で説明するスタイルシートファイルを指定します。
Doxygen が出力する LaTeX ソースファイルの冒頭に、
`\usepackage{doxygenjpext}` などが挿入されるようになります。

`doxygenjpext.sty` は Doxygen が出力した文書の、日本語に関する設定を行います。

`pdfinfo.sty` は、出力する PDF ファイルのプロパティ情報を設定する内容を記述します。

`fontsettings.sty` は Doxygen が出力した文書の、フォントに関する設定を行います。

`hyperlinkfix.sty` は、 PDF の関数名などのリンクをクリックしたときに、
正常にジャンプできるようにします。

### LATEX_EXTRA_FILES

    LATEX_EXTRA_FILES = book.cls

`refman.tex` LaTeX 文書のが使用するドキュメントクラスを、
`book` ではなく日本語に対応した `jsbook` を使用するように修正するための
偽装 `book` ドキュメントクラスファイルを指定します。

### PDF_HYPERLINKS

    PDF_HYPERLINKS = YES

PDF で HTML 出力のようなハイパーリンクを、ページ参照の代わりに出力するようにします。
Adobe Acrobat Reader のような PDF Viewer でブラウズするときに便利です。

この設定を行った上で、 `LATEX_EXTRA_STYLESHEET` に `hyperlikfix.sty` を指定することで、
ハイパーリンクをクリックすると参照先のページに移動できるようになります。

- - - - - - - - -

スタイルシートの用意
-------------------

以降で説明するスタイルシートファイルを、Doxyfile と同じディレクトリに置きます。

スタイルシートファイルは、 GitHub からダウンロードしてください。
ファイル名は [`doxygenjpext.sty`](doxygenjpext.sty)、
[`pdfinfo.sty`](pdfinfo.sty)、
[`fontsettings.sty`](fontsettings.sty)、
[`hyperlinkfix.sty`](hyperlinkfix.sty) です。

それらのうち、 `pdfnfo.sty` は、プロジェクト固有の情報を設定するため、
修正を行う必要があります。ファイルの内容は、以下のようになっているので、
`TITLE`, `PROJECT NAME`, `AUTHOR NAME`, `Doxygen 1.8.14`,
`KEYWORDS` を書き換えてください。

    \AtBeginDocument{\hypersetup{%
      pdfinfo={
        Title={TITLE},
        Subject={PROJECT NAME},
        Author={AUTHOR NAME},
        % Creator={},
        Producer={Doxygen 1.8.14},
        Keywords={KEYWORDS}%
      }%
    }}

- - - - - - - - -

PDF の作成
----------

準備ができたら、以下の手順で PDF を作成します。
(LaTeX 出力が `doc/latex` に出力されると想定しています。)

    cd /path/to/your/project
    doxygen && ( cd doc/latex && make && dvipdfmx refman )

- - - - - - - - -

内容の説明（何をしているのか）
---------------------------

以下に、各スタイルシートに記述する内容を説明します。

- - - - - - - - - - -

### 新日本語ドキュメントクラスを使用するように偽装する(`book.sty`)

Doxygen は出力する LaTeX 文書のドキュメントクラスに、
book クラスを使用するようにしています。

しかし、参考サイト
[LaTeX の「アレなデフォルト」傾向と対策](https://qiita.com/zr_tex8r/items/297154ca924749e62471)
にあるように、新日本語ドキュメントクラス jsbook を使用するように変更します。

そのためには、 book クラスの定義ファイルと同名のファイルを、
`refman.tex` と同じディレクトリに配置することで、
book クラスを偽装し、内部で jsbook クラスを読み込むことにします。

ただし、jsbook クラスでは、索引が3段組みになるので、
2段組になるように `report` オプションを指定するようにしました。

jsbook クラスでは余白が大きすぎるので、最小になるように設定し直しています。

また、Doxygen が使用している他のスタイルシートなどの影響でしょうか、
目次の章の行が章番号と見出しが重なってしまっています。
各章の見出しのページでは、章番号の後ろの「章」が出力されていません。
そこで、章番号の前後に「第」と「章」を出力しないようにしました。

book クラスのファイル `book.cls` は以下のようになります。

    \NeedsTeXFormat{LaTeX2e}
    \ProvidesClass{book}[2018/06/10 v0.1 Fake book class to generating Japanese PDF]

    % Parses and passes class options to the underlying jsbook class
    \DeclareOption*{\PassOptionsToClass{\CurrentOption}{jsbook}}
    \ProcessOptions

    % Load LaTeX's jsbook class with the uplatex and the dvipdfmx options
	\LoadClass[uplatex,dvipdfmx,report]{jsbook}%

	\renewcommand{\prechaptername}{}%
	\renewcommand{\postchaptername}{}%
    % Expand margins to fit doxygen output contents
	\setlength{\textwidth}{\fullwidth}%
	\setlength{\evensidemargin}{\oddsidemargin}%
	\setlength\footskip{2\baselineskip}%
	\addtolength{\textheight}{-2\baselineskip}%

- - - - - - - - - - -

### 日本語に関する設定を行う(`doxygenjpext.sty`)

`doxygenjpext.sty` に、日本語に関する設定を行うため、以下の内容を記述します。

#### パッケージに渡すドライバを指定する

各種パッケージに、 `dvipdfmx` または `dvipdfm` ドライバを指定します。

    \PassOptionsToPackage{dvipdfmx}{adjustbox}
    \PassOptionsToPackage{dvipdfmx}{color}
    \PassOptionsToPackage{dvipdfmx}{xcolor}
    \PassOptionsToPackage{dvipdfmx}{graphicx}
    \PassOptionsToPackage{dvipdfm}{geometry}

#### フォントメトリックの調整

和文フォントのメトリックを調整するために、`otf` パッケージなどを読み込みます。

    \usepackage[deluxe,jis2004,uplatex]{otf}
    \usepackage[T1]{fontenc}
    \usepackage{lmodern}

#### 和文対応としおり（ブックマーク）対応

Doxygen の出力した LaTeX ファイルでは、`hyperref` パッケージに `pdftex`
または `ps2pdf` ドライバが指定されています。これを `dvipdfmx` に変更します。

まず、`hyperref` パッケージを `dvipdfmx` ドライバを指定して読み込んで、
さらに和文に対応させるために、`pxjahyper` パッケージを読み込みます。
`pxjahyper` パッケージの最新版は、`hyperref` の　`unicode` オプションに
対応しているので、 `hyperref` を読み込むときに `unicode` を指定します。
(`pxjahyper` を読み込む前に指定しておかないと、しおりが文字化けしてしまいます。)

そして、このスタイルシート以降に現れた `\usepackage` に `hyperref` が指定されていたら、
無視するようにしてしまいます。

    \RequirePackage[dvipdfmx,pagebackref=true,unicode]{hyperref}
    \usepackage{pxjahyper}
    %
    \RequirePackage{xifthen}
    \RequirePackage{letltxmacro}
    \LetLtxMacro\originalusepackage\usepackage\relax
    \renewcommand\usepackage[2][]{%
      \ifthenelse{\equal{#2}{hyperref}}{}{\originalusepackage[#1]{#2}}%
    }

Doxygen の出力では、`LATEX_EXTRA_STYLESHEET` で指定したスタイルシート、
すなわち `doxygenjpext.sty` を読み込んだ後で
`hyperref` パッケージのオプションを指定しています。

そこで、プリアンブルの読み込みが終わってから、 `hyperref` のオプションを追加指定します。

    \AtBeginDocument{%
      \hypersetup{%
        bookmarks=true,bookmarksnumbered=true,% enable bookmarks
        setpagesize=false}%
    }

#### 索引が目次に2つ出力されるのを修正する

jsbook クラスを使うようにすると、目次に索引の行が2つ重複して出力されるようになってしまいます。

これは book や jbook クラスでは目次に索引が自動的には出力されないため、
Doxygen が明示的に目次の章を出力しているのですが、
jsbook クラスでは自動的に出力されるようにしているので2行出力されてしまいます。

そこで以下のように `printindex` マクロを再定義して、
jsbook 内で索引を目次に出力している `addcontentsline` マクロを何もしないようにします。

    \AtBeginDocument{%
      \LetLtxMacro\printindex\originalprintindex@fixaddcontentsline\relax
      \renewcommand\printindex{%
        \renewcommand\addcontentsline[3]{}%
        \originalprintindex@fixaddcontentsline}}

- - - - - - - - - - -

### PDF のプロパティに関する設定を行う(`pdfinfo.sty`)

`pdfinfo.sty` に PDF のプロパティに関する設定を行うため、以下の内容を記述します。
`TITLE`, `PROJECT NAME`, `AUTHOR NAME`, `Doxygen 1.8.14`,
`KEYWORDS` は、プロジェクト固有の情報に書き換えてください。

    \AtBeginDocument{\hypersetup{%
      pdfinfo={
        Title={TITLE},
        Subject={PROJECT NAME},
        Author={AUTHOR NAME},
        % Creator={},
        Producer={Doxygen 1.8.14},
        Keywords={KEYWORDS}%
      }%
    }}

- - - - - - - - - - -

### フォントに関する設定を行う(`fontosettings.sty`)

個人的な好みの設定になりますが、 `fontsettings.sty` に以下の内容を記述します。
(したがってこのスタイルシートは `LATEX_EXTRA_STYLESHEET` に書かなくても構いません。)

#### 見出しなどの和文書体をデフォルトの太字にする

見出しが太字になるようにします。

    \DeclareFontShape{JY2}{hmc}{bc}{n}{<->ssub*hmc/bx/n}{}
    \DeclareFontShape{JT2}{hmc}{bc}{n}{<->ssub*hmc/bx/n}{}
    \DeclareFontShape{JY2}{hgt}{bc}{n}{<->ssub*hgt/bx/n}{}
    \DeclareFontShape{JT2}{hgt}{bc}{n}{<->ssub*hgt/bx/n}{}

表紙の見出しはゴシックのボールド体にします。

    \RequirePackage{letltxmacro}
    \LetLtxMacro\originaltitlepage\titlepage\relax
    \renewenvironment{titlepage}{%
      \begin{originaltitlepage}\gtfamily\sffamily\bfseries%
    }{\end{originaltitlepage}}

#### 本文を明朝体に変更

Doxygen は書体にサンセリフ体を指定していますが、
和文の書体は指定していないためデフォルトの明朝体が使われてしまい、
欧文と和文がアンバランスになってしまいます。
そこで、本文の書体をローマンと明朝体に変更します。
ただし見出しとラベルはサンセリフ体にします。

    \AtBeginDocument{%
      % Change serif fonts in body text
      \renewcommand{\kanjifamilydefault}{\mcdefault}
      \renewcommand{\familydefault}{\rmdefault}
      % Select sanserif fonts in section headers
      \allsectionsfont{%
        \gtfamily\sffamily%
        \fontseries{bc}\selectfont%
        \color{darkgray}%
      }
      \renewcommand{\DoxyLabelFont}{%
        \gtfamily\sffamily%
        \fontseries{bc}\selectfont%
        \color{darkgray}%
      }
    }

- - - - - - - - - - -

### Doxygen の出力のハイパーリンクを修正する(`hyperlinkfix.sty`)

Doxygen の出力のハイパーリンクが正常に機能するように、
`hyperlinkfix.sty` に以下の内容を記述します。

私の環境のせいかもしれませんが、 Doxygen の出力から作成した PDF では、
関数名などのリンクをクリックしたときに、その定義場所にジャンプできていませんでした。
これはどうやら、Doxygen がリンクの参照に `\hyperlink` コマンドを使用しているせいのようです。
`\hyperlink` コマンドは、`\hypertarget` コマンドを使って参照先を設定するもののようですが、
Doxygen は参照先に `\label` コマンドを使用しているので、
dvipdfmx が PDF に変換するときに参照先が見つからないというエラーになってしまいます。

そこで `\hyperlink` コマンドを `\hyperref` コマンドにマクロ置換するようにします。
しかしながら、目次や索引の参照は `\hyperref` に置き換えるとリンクができないようでした。
そのため、 プリアンブルの `\makeindex` には影響がないようにするとともに、
`\tableofcontents` と `\printindex` の前後では、
マクロ置換を元に戻すようにしました。

それらを行うマクロが以下のようになります。

    \AtBeginDocument{%
      \LetLtxMacro\originalhyperlink\hyperlink\relax
      \def\hyperlinkexch#1#2{\hyperref[#1]{#2}}
      \LetLtxMacro\originaltableofcontents\tableofcontents\relax
      \def\tableofcontents{%
        \let\hyperlink\originalhyperlink\originaltableofcontents%
        \let\hyperlink\hyperlinkexch}%
      \LetLtxMacro\originalprintindex@hyperlinkfix\printindex\relax
      \renewcommand\printindex{%
        \let\hyperlink\originalhyperlink\relax\originalprintindex@hyperlinkfix%
        \let\hyperlink\hiperlinkexch}%
    }

- - - - - - - - -

最後に
-----

今回、このスタイルシートを作成する際には、冒頭の参考 URL のサイトの他にも
様々なサイトを参考にさせていただきました。

先人の方々には、大変感謝いたします。

また、私のブログでは kim 様からいろいろなアイデアをいただきました。
重ねて感謝の意を表します。
