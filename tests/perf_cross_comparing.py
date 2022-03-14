# Performance benchmark cross-comparing with a python CRDT library (https://github.com/anshulahuja98/python3-crdt)
# To run, install the following package through terminal:
# pip install python3-crdt

from py3crdt.gset import GSet
from py3crdt.gcounter import GCounter
from py3crdt.twopset import TwoPSet
from py3crdt.pncounter import PNCounter
from py3crdt.orset import ORSet
from py3crdt.lww import LWWElementSet as LWWSet
from py3crdt.sequence import Sequence
from datetime import datetime
import time
import uuid
from py3crdt.node import Node

if __name__ == '__main__':

    print("GSet Test")
    total = 0
    for x in range(10000):
        gset1 = GSet(id=1)
        gset2 = GSet(id=2)
        gset3 = GSet(id=3)
        gset1.add('a')
        gset1.add('b')
        gset1.add('d')
        gset1.add('e')
        gset2.add('b')
        gset2.add('c')
        gset2.add('e')
        gset3.add('x')
        gset3.add('y')
        gset3.add('o')
        t0 = time.time_ns()
        gset1.merge(gset2)
        gset1.merge(gset3)
        gset2.merge(gset1)
        gset2.merge(gset3)
        gset3.merge(gset1)
        gset3.merge(gset2)
        t1 = time.time_ns()
        total += (t1 - t0)
    average = total / 10000
    print("   Average Merging time: ", average, " ns")

    print("TwoPSet Test")
    total = 0
    for x in range(10000):
        TwoPSet1 = TwoPSet(id=1)
        TwoPSet2 = TwoPSet(id=2)
        TwoPSet3 = TwoPSet(id=3)
        TwoPSet1.add('a')
        TwoPSet1.add('b')
        TwoPSet1.add('d')
        TwoPSet1.add('e')
        TwoPSet1.remove('d')
        TwoPSet2.add('b')
        TwoPSet2.add('c')
        TwoPSet3.add('e')
        TwoPSet3.add('x')
        TwoPSet3.add('y')
        TwoPSet3.remove('e')
        TwoPSet3.add('o')
        t0 = time.time_ns()
        TwoPSet1.merge(TwoPSet2)
        TwoPSet1.merge(TwoPSet3)
        TwoPSet2.merge(TwoPSet1)
        TwoPSet2.merge(TwoPSet3)
        TwoPSet3.merge(TwoPSet1)
        TwoPSet3.merge(TwoPSet2)
        t1 = time.time_ns()
        total += (t1 - t0)
    average = total / 10000
    print("   Average Merging time: ", average, " ns")

    print("GCounter Test")
    total = 0
    for x in range(10000):
        gc1 = GCounter(id=1)
        gc1.add_new_node(key=10)
        gc1.add_new_node(key=11)
        gc1.add_new_node(key=12)
        gc1.inc(key=10)
        gc1.inc(key=10)
        gc1.inc(key=10)
        gc1.inc(key=11)
        gc1.inc(key=11)
        gc1.inc(key=11)
        gc1.inc(key=11)
        gc1.inc(key=12)
        gc1.inc(key=12)
        gc1.inc(key=12)
        gc1.inc(key=12)
        gc1.inc(key=12)

        gc2 = GCounter(id=2)
        gc2.add_new_node(key=10)
        gc2.add_new_node(key=11)
        gc2.add_new_node(key=12)
        gc2.inc(key=10)
        gc2.inc(key=10)
        gc2.inc(key=10)
        gc2.inc(key=10)
        gc2.inc(key=10)
        gc2.inc(key=10)
        gc2.inc(key=11)
        gc2.inc(key=11)
        gc2.inc(key=11)
        gc2.inc(key=12)
        gc2.inc(key=12)
        gc2.inc(key=12)
        gc2.inc(key=12)
        gc2.inc(key=12)
        gc2.inc(key=12)

        gc3 = GCounter(id=3)
        gc3.add_new_node(key=10)
        gc3.add_new_node(key=11)
        gc3.add_new_node(key=12)
        gc3.inc(key=10)
        gc3.inc(key=11)
        gc3.inc(key=11)
        gc3.inc(key=11)
        gc3.inc(key=11)
        gc3.inc(key=11)
        gc3.inc(key=11)
        gc3.inc(key=11)
        gc3.inc(key=11)
        gc3.inc(key=11)
        gc3.inc(key=12)
        gc3.inc(key=12)
        gc3.inc(key=12)
        gc3.inc(key=12)
        gc3.inc(key=12)

        t0 = time.time_ns()
        gc1.merge(gc2)
        gc1.merge(gc3)
        gc2.merge(gc1)
        gc2.merge(gc3)
        gc3.merge(gc1)
        gc3.merge(gc2)
        t1 = time.time_ns()
        total += (t1 - t0)

    average = total / 10000
    print("   Average Merging time: ", average, " ns")

    print("PNCounter Test")
    total = 0
    for x in range(10000):
        node1 = Node(uuid.uuid4())
        node2 = Node(uuid.uuid4())

        # Create a PNCounter
        pn1 = PNCounter(uuid.uuid4())

        # Add nodes to pn1
        pn1.add_new_node(node1.id)
        pn1.add_new_node(node2.id)

        # Increment pn1 values for each node
        pn1.inc(node1.id)
        pn1.inc(node1.id)
        pn1.inc(node1.id)
        pn1.inc(node1.id)
        pn1.inc(node1.id)
        pn1.inc(node1.id)
        pn1.inc(node1.id)

        # Decrement pn1 values for each node
        pn1.dec(node1.id)
        pn1.dec(node1.id)
        pn1.dec(node1.id)
        pn1.dec(node1.id)

        # Create another PNCounter
        pn2 = PNCounter(uuid.uuid4())

        # Add nodes to pn2
        pn2.add_new_node(node1.id)
        pn2.add_new_node(node2.id)

        # Increment pn2 values for each node
        pn2.inc(node1.id)
        pn2.inc(node2.id)
        pn2.inc(node2.id)
        pn2.inc(node2.id)

        # Decrement self.pn2 values for each node
        pn2.dec(node1.id)
        pn2.dec(node2.id)
        pn2.dec(node2.id)

        t0 = time.time_ns()
        pn1.merge(pn2)
        pn2.merge(pn1)
        t1 = time.time_ns()
        total += (t1 - t0)

    average = total / 10000
    print("   Average Merging time: ", average, " ns")

    print("ORSet Test")
    total = 0
    for x in range(10000):
        # Create a ORSet
        orset1 = ORSet(uuid.uuid4())

        # Create another ORSet
        orset2 = ORSet(uuid.uuid4())

        # Add elements to orset1
        orset1.add('a', uuid.uuid4())
        orset1.add('b', uuid.uuid4())
        orset1.add('x', uuid.uuid4())
        orset1.add('y', uuid.uuid4())
        orset1.add('z', uuid.uuid4())

        # Add elements to orset1
        orset2.add('b', uuid.uuid4())
        orset2.add('c', uuid.uuid4())
        orset2.add('d', uuid.uuid4())
        orset2.add('o', uuid.uuid4())

        orset1.remove('b')

        # Remove elements from orset2
        orset2.remove('b')
        orset2.remove('c')

        t0 = time.time_ns()
        # Merge orset2 to orset1
        orset1.merge(orset2)

        # Merge orset1 to orset2
        orset2.merge(orset1)
        t1 = time.time_ns()
        total += (t1 - t0)

    average = total / 10000
    print("   Average Merging time: ", average, " ns")

    print("LWW Test")
    total = 0
    for x in range(10000):
        # Create a LWWSet
        lww1 = LWWSet(uuid.uuid4())

        # Create another LWWSet
        lww2 = LWWSet(uuid.uuid4())

        # Add elements to lww1
        lww1.add('a')
        lww1.add('b')
        lww1.add('e')
        lww1.add('x')
        lww1.add('y')

        # Add elements to lww1
        lww2.add('b')
        lww2.add('c')
        lww2.add('d')
        lww2.add('o')

        # Remove elements from lww1
        lww1.remove('b')

        # Remove elements from lww2
        lww2.remove('b')
        lww2.remove('c')

        t0 = time.time_ns()
        # Merge lww2 to lww1
        lww1.merge(lww2)

        # Merge lww1 to lww2
        lww2.merge(lww1)
        t1 = time.time_ns()
        total += (t1 - t0)

    average = total / 10000
    print("   Average Merging time: ", average, " ns")

    print("Sequence (string) Test")
    total = 0
    for x in range(10000):
        # Create a Sequence
        seq1 = Sequence(uuid.uuid4())

        # Create another Sequence
        seq2 = Sequence(uuid.uuid4())

        # Add elements to seq1
        id1a = datetime.timestamp(datetime.now())
        seq1.add('a', id1a)
        id1b = datetime.timestamp(datetime.now())
        seq1.add('b', id1b)

        # Add elements to seq2
        id2c = datetime.timestamp(datetime.now())
        seq2.add('c', id2c)
        id2b = datetime.timestamp(datetime.now())
        seq2.add('b', id2b)
        id2d = datetime.timestamp(datetime.now())
        seq2.add('d', id2d)

        # Remove elements from seq1
        seq1.remove(id1b)

        # Remove elements from seq2
        seq2.remove(id2c)

        t0 = time.time_ns()
        # Merge seq2 to seq1
        seq1.merge(seq2)

        # Merge seq1 to seq2
        seq2.merge(seq1)
        t1 = time.time_ns()
        total += (t1 - t0)

    average = total / 10000
    print("   Average Merging time: ", average, " ns")

