/*
Copyright (c) of 2017 by Fan Zhang <fanzhang.cs@gmail.com>
*/

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<vector>
#include<time.h>
#include<iostream>
#include<algorithm>
#include <cstdlib>
#include <ctime>

using namespace std;

long inputK, inputB, datasetID, inikcoreSize, inidegreekNeiSize, visitNumber = 0;
double followerNumber = 0;
string infile, outfile;
vector<long> degreekVertices, degreekNeighbors, degreekNeiTag, kcoreDeletedTag, degreekVerticesNew, followerTag;//set P and T
vector<long> verSetTag, verOrigIDs, kcoreOrgIDs, verTag, kcoreTag, verDegree, lesskSet, kcoreDegree, followersRecord, kcoreSetIDs;
vector<long> collapserIDs, numFollowersRecord;
vector<vector<char> > verNames;
vector<vector<long> > verSet0, verSet, kcoreSet, verNameIDs;



void dataInput()
{
	double time0 = (double)clock() / CLOCKS_PER_SEC;

	//read edges, build verSet //need first row ordered data
	long vertexID, neighborID;
	vector<long> verSetInsertion;
	FILE *fe = NULL;
	fe = fopen(infile.c_str(), "r");
	if (fe == NULL)
	{
		printf("ERROR!");
		exit(1);
	}
	else
	{
		long verid = -1, vid, nid;
		char fech = 's';
		while (fech != '\377')
		{
			vertexID = 0;
			fech = getc(fe);
			if (fech == '\377') break;
			while (fech != '\t')
			{
				vertexID = vertexID * 10 + (int)fech - 48;
				fech = getc(fe);
			}
			neighborID = 0;
			fech = getc(fe);
			while (fech != '\n' && fech != '\377')
			{
				neighborID = neighborID * 10 + (int)fech - 48;
				fech = getc(fe);
			}

			vid = verSetTag[vertexID];
			nid = verSetTag[neighborID];
			if (vid < 0)
			{
				verid++;
				verSetTag[vertexID] = verid; //vertexID -> vid
				verOrigIDs.push_back(vertexID); //vid -> vertexID
				vid = verid;
			}
			if (nid < 0)
			{
				verid++;
				verSetTag[neighborID] = verid;
				verOrigIDs.push_back(neighborID);
				nid = verid;
			}
			if (verSet0.size() == 0)
			{
				verSetInsertion.clear();
				verSetInsertion.push_back(vid);
				verSetInsertion.push_back(nid);
				verSet0.push_back(verSetInsertion);
			}
			else
			{
				if (vid == verSet0[verSet0.size() - 1][0])
				{
					verSet0[verSet0.size() - 1].push_back(nid);
				}
				else
				{
					verSetInsertion.clear();
					verSetInsertion.push_back(vid);
					verSetInsertion.push_back(nid);
					verSet0.push_back(verSetInsertion);
				}
			}
		}
	}
	fclose(fe);
	verSetInsertion.clear();
	for (unsigned long i = 0; i < verSet0.size(); i++)
	{
		kcoreSetIDs.push_back(-1);
		verSet.push_back(verSetInsertion);
	}
	for (unsigned long i = 0; i < verSet0.size(); i++)
	{
		long id = verSet0[i][0];
		for (unsigned long j = 1; j < verSet0[i].size(); j++)
		{
			verSet[id].push_back(verSet0[i][j]);
		}
	}
	long degree;
	long maxDegree = 0;
	double averageDegree = 0;
	lesskSet.clear();
	for (unsigned long i = 0; i < verSet.size(); i++)
	{
		degree = verSet[i].size();
		verDegree.push_back(degree);
		if (degree < inputK)
		{
			verTag.push_back(2);
			lesskSet.push_back(i);
		}
		else
		{
			verTag.push_back(1);
		}
		if (degree > maxDegree)
		{
			maxDegree = degree;
		}
		averageDegree += degree;
	}

	averageDegree /= (double)(verSet.size());
	printf("max Degree: %ld\naverage Degree: %lf\n", maxDegree, averageDegree);
	double time1 = (double)clock() / CLOCKS_PER_SEC;
	printf("read time: %lf\n", time1 - time0);

}


void dataOutput(double &runtime)
{
	char* writeTemp;
	char record[100];
	FILE *fs;
	fs = fopen(outfile.c_str(), "a");
	char fsch;

	if (fs == NULL)
	{
		printf("ERROR!");
		exit(1);
	}
	else
	{
		
		sprintf(record, "k%ld", inputK);
		fwrite(record, sizeof(*record), strlen(record), fs);
		fsch = putc('\t', fs);
		sprintf(record, "b%ld", inputB);
		fwrite(record, sizeof(*record), strlen(record), fs);
		fsch = putc('\t', fs);
		sprintf(record, "%.2lf", runtime);
		fwrite(record, sizeof(*record), strlen(record), fs);
		fsch = putc('\t', fs);
		sprintf(record, "%.2lf", followerNumber);
		fwrite(record, sizeof(*record), strlen(record), fs);
		fsch = putc('\t', fs);
		sprintf(record, "%ld", visitNumber);
		fwrite(record, sizeof(*record), strlen(record), fs);
		fsch = putc('\t', fs);

		sprintf(record, "kc%ld", inikcoreSize);
		fwrite(record, sizeof(*record), strlen(record), fs);
		fsch = putc('\t', fs);
		sprintf(record, "kn%ld", inidegreekNeiSize);
		fwrite(record, sizeof(*record), strlen(record), fs);
		fsch = putc('\t', fs);

		for (unsigned long i = 0; i < collapserIDs.size(); i++)
		{
			long id = collapserIDs[i];
			if (id != -1)
			{
				fsch = putc('\t', fs);
				sprintf(record, "c%ld", kcoreOrgIDs[id]);
				fwrite(record, sizeof(*record), strlen(record), fs);
			}
			else
			{
				fsch = putc('\t', fs);
				sprintf(record, "X");
				fwrite(record, sizeof(*record), strlen(record), fs);
			}
		}
		fsch = putc('\n', fs);
	}
}

void buildKcore()
{
	//kcorePruNum = 0;
	for (unsigned long i = 0; i < lesskSet.size(); i++)
	{
		long id = lesskSet[i];
		//kcorePruNum++;
		verTag[id] = 0; //cur vertex deleted
		verDegree[id] = 0;
		for (unsigned long j = 0; j < verSet[id].size(); j++)
		{
			long nid = verSet[id][j];
			if (verTag[nid] == 1)
			{
				verDegree[nid]--; //neighbor degree - 1
				if (verDegree[nid] < inputK) //new candidate for computing
				{
					lesskSet.push_back(nid);
					verTag[nid] = 2;
				}
			}
		}
	}
	kcoreTag.clear();
	kcoreSet.clear();
	vector<long> kcoreInsertion;
	long ki = -1;
	for (unsigned long i = 0; i < verTag.size(); i++)
	{
		if (verTag[i])
		{
			ki++;
			kcoreSetIDs[i] = ki;
			degreekNeiTag.push_back(0);
			//kcoreDeletedTag.push_back(0);
		}
	}
	for (unsigned long i = 0; i < verTag.size(); i++)
	{
		if (verTag[i])
		{
			kcoreInsertion.clear();
			for (unsigned long j = 0; j < verSet[i].size(); j++)
			{
				long nid = verSet[i][j];
				if (verTag[nid])
				{
					long knid = kcoreSetIDs[nid];
					kcoreInsertion.push_back(knid);
				}
			}
			long deg = verDegree[i];
			kcoreDegree.push_back(deg);
			kcoreTag.push_back(1);
			kcoreSet.push_back(kcoreInsertion);
			kcoreOrgIDs.push_back(verOrigIDs[i]); //kid -> vid -> vertexID
			followerTag.push_back(1);
			numFollowersRecord.push_back(0);
			if (deg == inputK)
			{
				degreekVertices.push_back(kcoreSetIDs[i]);
			}
		}
	}
	inikcoreSize = kcoreSet.size();
	for (unsigned long i = 0; i < degreekVertices.size(); i++)
	{
		long id = degreekVertices[i];
		for (unsigned long j = 0; j < kcoreSet[id].size(); j++)
		{
			long nid = kcoreSet[id][j];
			if (!degreekNeiTag[nid])
			{
				degreekNeighbors.push_back(nid);
				degreekNeiTag[nid] = 1;
			}
		}
	}
	for (unsigned long i = 0; i < degreekNeighbors.size(); i++)
	{
		long deg = 0;
		long id = degreekNeighbors[i];
		for (unsigned long j = 0; j < kcoreSet[id].size(); j++)
		{
			long nid = kcoreSet[id][j];
			if (degreekNeiTag[nid])
			{
				deg++;
			}
		}
		degreekNeiTag[id] = deg;
	}
	inidegreekNeiSize = degreekNeighbors.size();
}

long CKCalgorithm()
{
	long maxNum = -1, bestCollapser = -1;
	vector<long> degreeChangedVertices, kcoreDegree0 = kcoreDegree;
	for (unsigned long i = 0; i < degreekNeighbors.size(); i++)
	{
		long id = degreekNeighbors[i];

		if (kcoreTag[id] && followerTag[id])
		{
			visitNumber++;
			//compute collapsed kcore
			lesskSet.clear();
			lesskSet.push_back(id);
			degreeChangedVertices.clear();
			for (unsigned long k = 0; k < lesskSet.size(); k++)
			{
				long lid = lesskSet[k];
				//kcorePruNum++;
				followerTag[lid] = 0;
				kcoreTag[lid] = 0; //cur vertex deleted
				kcoreDegree[lid] = 0;
				for (unsigned long j = 0; j < kcoreSet[lid].size(); j++)
				{
					long nid = kcoreSet[lid][j];
					if (kcoreTag[nid] > 0)
					{
						kcoreDegree[nid]--; //neighbor degree - 1
						if (kcoreTag[nid] < 2)
						{
							kcoreTag[nid] = 2;
							degreeChangedVertices.push_back(nid);
						}
						if (kcoreDegree[nid] < inputK) //new candidate for computing
						{
							lesskSet.push_back(nid);
							kcoreTag[nid] = -1;
						}

					}
				}
			}
			long numFollowers = lesskSet.size() - 1;
			numFollowersRecord[id] = numFollowers;
			if (numFollowers > maxNum)
			{
				maxNum = numFollowers;
				bestCollapser = id;
			}
			followerTag[id] = 1;
			kcoreTag[id] = 1;
			kcoreDegree[id] = kcoreDegree0[id];
			for (unsigned long j = 0; j < degreeChangedVertices.size(); j++)
			{
				long did = degreeChangedVertices[j];
				kcoreDegree[did] = kcoreDegree0[did];
				kcoreTag[did] = 1;
			}
		}
	}
	return bestCollapser;
}

long updateKcoreAndCandidates(long bestCollapser)
{
	vector<long> degreeChangedVertices;
	//compute collapsed kcore
	lesskSet.clear();
	lesskSet.push_back(bestCollapser);
	degreeChangedVertices.clear();
	for (unsigned long i = 0; i < lesskSet.size(); i++)
	{
		long lid = lesskSet[i];
		//kcorePruNum++;
		kcoreTag[lid] = 0; //cur vertex deleted
		kcoreDegree[lid] = 0;
		if (i)
		{
			followersRecord.push_back(lid); //vertexID
		}
		if (degreekNeiTag[lid])
		{
			degreekNeiTag[lid] = 0;
		}
		for (unsigned long j = 0; j < kcoreSet[lid].size(); j++)
		{
			long nid = kcoreSet[lid][j];
			if (kcoreTag[nid] > 0)
			{
				kcoreDegree[nid]--; //neighbor degree - 1
				if (kcoreTag[nid] < 2)
				{
					kcoreTag[nid] = 2;
					degreeChangedVertices.push_back(nid);
				}
				if (kcoreDegree[nid] < inputK) //new candidate for computing
				{
					lesskSet.push_back(nid);
					kcoreTag[nid] = -1;
				}
			}
		}
	}
	for (unsigned long i = 0; i < degreeChangedVertices.size(); i++)
	{
		long id = degreeChangedVertices[i];
		if (kcoreDegree[id] == inputK)
		{
			degreekVertices.push_back(id);
			for (unsigned long j = 0; j < kcoreSet[id].size(); j++)
			{
				long nid = kcoreSet[id][j];
				if (!degreekNeiTag[nid] && kcoreTag[nid])
				{
					degreekNeighbors.push_back(nid);
					degreekNeiTag[nid] = 1;
				}
			}
		}
		if (kcoreTag[id] == 2)
		{
			kcoreTag[id] = 1;
		}
	}

	return lesskSet.size() - 1;
}

bool compare(const long &a, const long &b)//sort comparison for degree decreasing
{
	return degreekNeiTag[a] > degreekNeiTag[b];
}

int main(int argc, char *argv[])
{
	long collapserID;

	//configure data
	long maxVerID = 100000000; //max vertex id
	verSetTag.resize(maxVerID, -1);
	infile = "dataset.txt";
	outfile = "result.txt";
	scanf("%ld %ld", &inputK, &inputB);

	//input data 
	dataInput();

	//algorithm
	double algStartTime = (double)clock() / CLOCKS_PER_SEC;
	buildKcore();
	sort(degreekNeighbors.begin(), degreekNeighbors.end(), compare);
	for (long i = 0; i < inputB; i++)
	{
		collapserID = CKCalgorithm();
		collapserIDs.push_back(collapserID);
		if (collapserID != -1)
		{
			followerNumber += updateKcoreAndCandidates(collapserID);
			sort(degreekNeighbors.begin(), degreekNeighbors.end(), compare);
		}
	}
	double runtime = (double)clock() / CLOCKS_PER_SEC - algStartTime;
	printf("time: %lf\n", runtime);

	//write
	dataOutput(runtime);

	return 0;
}
