from brian import Connection, DelayConnection

def brianConnectionToFile(connection, weightfactor, filename):
	if not isinstance(connection, Connection):
		raise TypeError("argument must be brian Connection")

	isConstDelay = False
	constDelay = 0
	if not isinstance(connection, DelayConnection):
		isConstDelay = True
		constDelay = connection.delay

	fromSize = len(connection.source)
	fromStart = connection.source._origin
	toSize = len(connection.target)
	toStart = connection.target._origin
	#startID = connection.source
 
	with open(filename, 'w') as f:
		f.write("# source: "+str(connection.source)+"\n")
		f.write("# target: "+str(connection.target)+"\n")
		for con_source in range(0, fromSize):
			for con_target in range(0, toSize):
				if connection[con_source, con_target] != 0:
					delay = constDelay
					if not isConstDelay:
						delay = connection.delay[con_source, con_target]
					sourceID = fromStart + con_source
					targetID = toStart + con_target
					weigth = connection[con_source, con_target] * weightfactor
					f.write(str(sourceID)+"\t"+str(targetID)+"\t"+str(weigth)+"\t"+str(delay)+"\n")
		f.close()
