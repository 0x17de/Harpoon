if [ "x$1" == "x" ]; then
	echo "Generates Files: Event\${ARG1}.{cpp,h}"
fi

sed -r "s/TEMPLATE/\\U${1}/g; s/Template/${1}/g" EventTemplate.cpp > Event${1}.cpp
sed -r "s/TEMPLATE/\\U${1}/g; s/Template/${1}/g" EventTemplate.h > Event${1}.h
