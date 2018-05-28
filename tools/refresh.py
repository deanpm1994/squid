#!/usr/bin/env python

import psycopg2 as db
import os, sys, time

connection_info = dict(database="proxy_db",user="proxy",password="yoidUct0", host="10.6.143.9")
connection = None

direction = ''
directions = ['/var/squid/logs/','/var/log/squid/','/usr/local/squid/var/logs/']
lines = []
for path in directions:
    if os.path.exists(str(path+'access.log')):
	direction = path
        f = open(path+'access.log', 'r')
        lines = list(f)
        f.close()
if lines != []:
    quotas = {}

    if not os.path.exists(direction+'.lastline'):
        with open(direction+'.lastline', 'w') as lastline:
            lastline.write("-1\n")
	    lastline.write(lines[0])
            lastline.write("-1\n")
	    lastline.write(time.ctime())
        
    flast = open(direction+'.lastline', 'r')
    data = list(flast)
    first = data[1]
    last = int(data[0].strip())
    flast.close()
    print first
    print lines[0]
    if first != lines[0]:
        with open(direction+'.lastline', 'w') as lastline:
            lastline.write("-1\n")
	    lastline.write(lines[0])
            lastline.write("-1\n")
	    lastline.write(time.ctime())
        last = -1
    else:
	print "No Changes"

    print last
    if last == -1:
	print "Nuevo"
        last = 0
    for line in lines[last+1:]:
        try:
		date, duration, ip, status, bytesc, oper, url, user, method, cont_type = line.split()
        	if user != '-' and not status.__contains__('TCP_DENIED/') and not status.__contains__('NONE/'):
            		if not quotas.has_key(user):
                		quotas[user] = int(bytesc)
            		else:
                		quotas[user] += int(bytesc)
        except:
		print "Hey"
		print line
	last += 1

    with open(direction+'.lastline', 'w') as lastline:
        lastline.write(str(last) + "\n")
	lastline.write(lines[0])
        lastline.write(str(last) + "\n")
	lastline.write(time.ctime())
    # print quotas

    for user in quotas.keys():
	print user
        query = "SELECT consumido FROM estudiantes_dean WHERE correo = %s"
        query_args = (str(user),)
        try:
            if connection is None:
                connection = db.connect(**connection_info)
            cursor = connection.cursor()
            cursor.execute(query, query_args)
        except Exception, e:
            connection = db.connect(**connection_info)
            cursor = connection.cursor()
            cursor.execute(query, query_args)
        try:
            row = cursor.fetchone()
            if row is not None:
                print quotas[user] + int(row[0])
                query = "UPDATE estudiantes_dean SET consumido = %s WHERE correo = %s"
                query_args = (quotas[user] + int(row[0]), user)
                cursor.execute(query, query_args)
                connection.commit()
            else:
                print "Something wrong"
                sys.exit(2)
        except:
            print "Something wrong"
            sys.exit(2)
        finally:
            if connection:
                print "Finally"
                connection.close()
else:
    print "No access.log...Doing nothing"
