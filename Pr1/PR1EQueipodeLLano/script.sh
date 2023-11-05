if [-e student-record.c]; then
	if [-x student-record.c]; then
		if [-e records.txt] then
			if [-f records.txt] then
				records = $(cat records.txt)
				declare -i i=0
				gcc -o student student-record
				for entry in records
				do
					if [i -eq 0]; then
						./student -f database -c entry
					else
						./student -f database -a entry
					i+=1
					fi
				done
				./student -f database -l
				xxd database
				for entry in records
				do
					./student -f database -q -n $(cut -d ':' -f2)
				done
				
			else	
				echo "records.txt no es un fichero regular"
			fi
		else
			echo "records.txt no existe"
		fi
	else
		echo “student-record no es ejecutable”
	fi
else
	echo "student-record no existe"
fi
