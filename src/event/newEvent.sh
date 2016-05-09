if [ "x$1" == "x" ]; then
	echo "Generates Files: Event\${ARG1}.{cpp,hpp}"
fi

sed -r "s/TEMPLATE/\\U${1}/g; s/Template/${1}/g" EventTemplate.cpp > Event${1}.cpp
sed -r "s/TEMPLATE/\\U${1}/g; s/Template/${1}/g" EventTemplate.hpp > Event${1}.hpp
