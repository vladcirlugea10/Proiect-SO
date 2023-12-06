#!/bin/bash

if [ "$#" -ne 1 ]; then
    echo "Numar gresit de argumente"
    exit 1
fi

caracter="$1"
propozitii_corecte=0

while IFS= read -r line || [[ -n "$line" ]]; do
    if echo "$line" | grep -Eq "^[A-Z][a-zA-Z0-9 ,]*[\.\?\!]$"; then
        if ! echo "$line" | grep -Eq "(,|^)[ ]*si"; then
            if echo "$line" | grep -qi "$caracter"; then
                ((propozitii_corecte++))
            fi
        else
            echo "Propozitia contine ',' inainte de 'si'!"
        fi
    else
        echo "Propozitia nu este corecta!"
    fi
done

echo "$propozitii_corecte"


