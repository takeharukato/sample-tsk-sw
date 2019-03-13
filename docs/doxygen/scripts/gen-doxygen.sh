#!/bin/bash
#
# Copyright 2019 Takeharu KATO
#
# gen-doxygen.sh [-h]
#                [-s プログラムソースディレクトリ] 
#                [-d doxygenのテンプレートディレクトリ]
#                [-n アーカイブファイル名]
#                [-o 出力先ディレクトリ]
# 実行するとカレントディレクトリにdoxygen-docs.tar.gzというdoxygenの出力結果アーカイブができる
#
# 事前にGraphvizが必要. Doxygen-1.8.15で動作を確認
# sudo yum install -y graphviz
#
SCRIPT_DIR=$(cd $(dirname $0); pwd)
DOXYGEN_DOCS_DIR=$(cd ${SCRIPT_DIR}/..; pwd) #doxygenディレクトリ
DOXYGEN_DOCS_STYLE_FILE_DIR=styles
DOXYGEN_DOCS_EXTRA_FILE_DIR=styles
DOXYGEN_DOCS_TEMPLATES_DIR=templates
DOXYGEN_OUTPUT_DIR=doxygen-outputs
DOXYGEN_DOCS_ARCHIVE_NAME=doxygen-docs
DOXYGEN_DOCS_TEMPL=Doxyfile.template
DOXYGEN_DOCS_LOGNAME="doxygen-error.log"

gen_doxyfile() {
    local template
    local doxygen_dir
    local output
    local file
    local style_line
    local extra_file_line
    local output_dir
    local name
    local project_num

    if [ $# -ne 4 ]; then
	echo "gen_doxyfile doxygendir output outdir name"
	exit 1
    fi

    doxygen_dir=$1
    output=$2
    output_dir=$3
    name=$4

    template="${DOXYGEN_DOCS_DIR}/${DOXYGEN_DOCS_TEMPLATES_DIR}/${DOXYGEN_DOCS_TEMPL}"
    style_line=""
    extra_file_line=""

    for file in doxygenjpext.sty fontsettings.sty hyperlinkfix.sty pdfinfo.sty
    do
	if [[ ! "x${file}" =~ "[[:blank:]]*x[[:blank:]]*" ]]; then
	    style_line="${style_line} ${DOXYGEN_DOCS_DIR}/${DOXYGEN_DOCS_STYLE_FILE_DIR}/${file}"
	fi
    done

    for file in book.cls  
    do
	if [[ ! "x${file}" =~ "[[:blank:]]*x[[:blank:]]*" ]]; then
	    extra_file_line="${extra_file_line} ${DOXYGEN_DOCS_DIR}/${DOXYGEN_DOCS_EXTRA_FILE_DIR}/${file}"
	fi
    done
    
    project_num=`git log -n 1 --format=%H`

    sed -e "s|__LTX_STYLE_SHEET__|${style_line}|" \
	-e "s|__LTX_EXTRA_FILES__|${extra_file_line}|g" \
	-e "s|__OUTPUT_DIR__|${output_dir}|g" \
	-e "s|__PROJECT__|The Detail Design of ${name}|g" \
	-e "s|__PROJECT_NUMBER__|Revision ID: ${project_num}|g" \
	${template} >  ${output}

    diff -u ${template} ${output}
}

doxygen_docs(){
    local topdir
    local tmpfile=`mktemp`
    local doxygen_dir
    local src_dir
    local output_dir
    local base_archive_name
    local archive_name
    local doxygen_log
    local cwd

    if [ $# -ne 4 ]; then
	echo "doxygen_docs doxygen-dir src-dir output-dir archive-name"
	exit 1
    fi

    doxygen_dir=$1
    src_dir=$2
    output_dir=$3
    base_archive_name=$4
    cwd=`pwd`

    doxygen_log="${cwd}/${base_archive_name}-${DOXYGEN_DOCS_LOGNAME}"
    archive_name="${base_archive_name}-doxygen-doc"

    if [ ! -d "${output_dir}" -a "x${output_dir}" != "x" ]; then
	mkdir -p "${output_dir}"
    fi

    pushd "${src_dir}" 1> /dev/null 2> /dev/null
    
    gen_doxyfile "${doxygen_dir}" "${tmpfile}" "${output_dir}" "${base_archive_name}"

    if [ -f "${doxygen_log}" ]; then
	rm -f "${doxygen_log}"
    fi
    
    { { doxygen "${tmpfile}" >&3; } 2>&1 |tee "${doxygen_log}" 1>&2;} 3>&1

    mv "${output_dir}/latex/refman.tex" "${output_dir}/latex/refman.tex.orig"
    sed -e 's| *\\newunicodechar.*||g' -e 's|pdftex|dvipdfmx|g' -e 's|ps2pdf|dvipdfmx|g' \
	"${output_dir}/latex/refman.tex.orig" > "${output_dir}/latex/refman.tex"
    
    pushd "${output_dir}/latex" 1> /dev/null 2> /dev/null
    gmake && dvipdfmx refman
    popd 1> /dev/null 2> /dev/null

    popd 1> /dev/null 2> /dev/null

    pushd "${output_dir}" 1> /dev/null 2> /dev/null
    tar zcf "${cwd}/${archive_name}.tar.gz" .
    popd 1> /dev/null 2> /dev/null

    echo "${cwd}/${archive_name}.tar.gz has been created."
}

show_help() {

    echo "gen-doxygen.sh [-h]  [-s source-dir] [-d doxygen-template-dir] [-o output-dir]"
    exit 1
}

remove_dir(){
    local arg
    local file
    local dir
    local full

    if [ $# -ne 1 ]; then
	echo "remove_dir dirctory"
	exit 1
    fi

    # ディレクトリの絶対パスを得る
    arg=$1
    if [ ! -d "${arg}" ]; then
	dir=$(cd $(dirname "${arg}") && pwd)
    else
	dir=$(cd "${arg}" && pwd)
    fi
    if [ ! -d "${dir}" ]; then
	return
    fi
    echo "Remove: ${dir}"
    if [ -d "${dir}" ]; then
	find "${dir}"|sort -r |while read file
	do
	    # echo "Remove ${file}"
	    if [ -d  "${file}" ]; then
		if [[ ${full} =~ ^${dir} ]]; then
		    rm -fr "${file}"
		fi
	    else
		if [ -e "${file}" ]; then
		    rm -f  "${file}"
		fi
	    fi
	done
    fi
}
main(){
    local OPT
    local doxgen_dir
    local output_dir
    local opt_output_dir
    local opt_archive_name
    local base_archive_name
    local src_dir 
    local tempdir
    local file

    tempdir=`mktemp -d`
    mkdir -p ${tempdir}

    opt_archive_name=""
    doxygen_dir="${DOXYGEN_DOCS_DIR}"
    src_dir=`pwd`
    output_dir="${tempdir}/${DOXYGEN_OUTPUT_DIR}"

    while getopts "hd:n:s:o:" OPT
    do
	case $OPT in
	    "s" )
		src_dir="${OPTARG}"
		if [ ! -d "${src_dir}" ]; then
		    echo "Source Dir: ${src_dir} is not directory."
		    exit 1
		fi
		src_dir=$(cd "${src_dir}" && pwd)
		;;
            "d" )
		doxygen_dir="${OPTARG}"
		if [ ! -d "${doxygen_dir}" ]; then
		    echo "Doxygen Template Dir: ${doxygen_dir} is not directory."
		    exit 1
		fi
		doxygen_dir=$(cd "${doxygen_dir}" && pwd)
		;;
	    "n" )
		opt_archive_name="${OPTARG}"
		;;
            "o" ) 
		opt_output_dir="${OPTARG}"
		if [ ! -d "${opt_output_dir}" ]; then
		    echo "Output Dir: ${opt_output_dir} is not directory."
		    exit 1
		fi
		output_dir=$(cd "${opt_output_dir}" && pwd)
		;;
	    "h" )
		show_help
		exit 0
		;;
	esac
    done

    if [ "x${opt_archive_name}" != "x" ]; then
	base_archive_name="${opt_archive_name}"
    else
	base_archive_name=`basename "${src_dir}"`
    fi

    if [ "x${doxygen_dir}" = "x" -o ! -d "${doxygen_dir}" ]; then
	echo "Internal error: Invalid doxygen directory: ${doxygen_dir}"
	exit 1
    fi

    if [ "x${src_dir}" = "x" -o ! -d "${src_dir}" ]; then
	echo "Invalid source directory: ${src_dir}"
	exit 1
    fi

    if [ -d "${output_dir}" ]; then
	remove_dir "${output_dir}"
    fi

    if [ "x${output_dir}" != "x" -a ! -d "${output_dir}" ]; then
	mkdir -p "${output_dir}"
    fi

    doxygen_docs "${doxygen_dir}" "${src_dir}" "${output_dir}" "${base_archive_name}"

    if [ -d "${tempdir}" ]; then
	remove_dir "${tempdir}"
    fi
}

main $@
