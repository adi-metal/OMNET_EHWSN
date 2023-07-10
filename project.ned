//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

//package hello_world;

//
// TODO auto-generated type
//
network N1
{
    parameters:
        int numNodes;
    submodules:
        source: MySource;
        node[numNodes]: MyNode;
        wireless: MyWireless;
        sink: MySink;
}


simple MySource
{
    parameters:
        double x;
        double y;
        double transmission_range;
        int num_nodes = parent.numNodes;
        @display("i=block/source");
}

simple MyNode
{
    parameters:
       int key;
       double x;
       double y;
    gates:
       input node_in @directIn;
}


simple MyWireless
{
    gates:
        input in @directIn;
}

simple MySink
{
    parameters:
        double x;
        double y;

    gates:
        input sink_in @directIn;
}
