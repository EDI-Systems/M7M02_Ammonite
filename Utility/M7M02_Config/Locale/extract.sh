CPP_FILE_LIST=`find .. \( -name "*.cpp" -or -name "*.hpp" \) -print`
xgettext -d sdm_dl -s --keyword=_ -p . -o sdm_dl.pot $CPP_FILE_LIST
