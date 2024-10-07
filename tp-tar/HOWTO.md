

# Comment tester vos réponses

Pour compiler les tests, saisissez la commande 

    make test

Si vous voulez lancer les tests, saisissez la commande :

    make runtests

Si vous voulez simplement lancer un sous-ensemble des tests, vous
devez d'abord vous déplacer dans le repertoire `test`, ensuite lancer
la commande `test_ex1` pour les tests de l'exercice 1:

    cd test && ./test_ex1

Si vous désirez afficher la liste des tests sans les lancer, tapez :

    ./test_ex1 -l -v

Si vous souhaitez lancer seulement les tests relatifs à la question 2
de l'exercice 1, tapez :

    ./test_ex1 ex1.q2 

Des commandes similaires sont disponibles pour l'exercice 2.

