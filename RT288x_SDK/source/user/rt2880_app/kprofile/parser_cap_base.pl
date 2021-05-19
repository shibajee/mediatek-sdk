#!/usr/bin/perl

open raw_input, $ARGV[0] 
  or die "open source file fail($!)";

$header_part = ".$ARGV[0].header";
$result_part = ".$ARGV[0].result";
$body_part   = ".$ARGV[0].body";
$tmp = $ARGV[0];
$items = 24;
#$log_overhead = 0;
$log_overhead = 173;

$_ = $ARGV[1];

if ( $_ =~ /[a-zA-Z]+/ )
{
    die "Input should be a number\n";
}

$items = $ARGV[1];

$_ = $ARGV[0];

if ( $_ =~ /.csv/ )
{
    $tmp=~s/.csv/_result.csv/;
}
else
{
    $tmp .= "_result.csv";
}

$target_file = $tmp;

$success = open HEADER, ">",  $header_part;

unless ( $success )
{
    $HEADER = STDOUT;
}

$success = open BODY, ">",  $body_part;

unless ( $success )
{
    $BODY = STDOUT;
}

$success = open RESULT, ">",  $result_part;

unless ( $success )
{
    $RESULT = STDOUT;
}

@string_array = ();

while (<raw_input>)
{
    chomp;

    push @string_array, $_;
}

$total_record = 0;

@cycle_list      = ();
@inst_list       = ();
@name_list       = ();
@id_list         = ();
@ipc_sum         = ();
@inst_sum        = ();

$item_count = 0;
$process_name = 0;
$process_data = 0;
$header_print = 0;
$entry_count = 0;

foreach $line (@string_array)
{
#    print "$line\n";    
    @item_list = split /\s+/, $line;

    $v0 = $item_list[0];
    $v1 = hex $item_list[2];
    $v2 = hex $item_list[3];
    $v3 = hex $item_list[4];

    if ( $process_name == 1 )
    {
        $_ = $item_list[1];

        if ( /.+/ )
        {
            #if ( $v0 =~ /[0-9]+/ )
            if ( $v0 =~ /^[+\-]?([1-9]\d*|0)(\.\d+)?([eE][+\-]?([1-9]\d*|0)(\.\d+)?)?$/ )
            {
                $name_list[$v0] = $item_list[1]." ".$item_list[2];
				#print "name_list[$v0]=$name_list[$v0]\n";    
            }
        }
        else
        {
            $process_name = 0;

			#foreach $name (@name_list)
			#{
			#    print "[$name]\n";
			#}
        }
    }
    else
    {
        if ( $process_data == 1 )
        {
            if ( ( $v1 != 0 ) && ( $v2 != 0 ) )
            {
                push @cycle_list, $v1;
                push @inst_list, $v2;
                push @id_list, $item_list[4];
                $item_count++;

                if ( $v3 == 1 )
                {
                    if ( $item_count == $items )
                    {
                        if ( $header_print == 0 )
                        {
                            $loop = 0;
                            select HEADER;
                            print "FROM,";
                            while ( $loop < ( $item_count - 1 ) )
                            {
                                print "$name_list[$id_list[$loop]],";
                                $loop++;
                            }

                            print "\n";
                            $loop = 0;
                            print "TO,";
                            while ( $loop < ( $item_count - 1 ) )
                            {
                                print "$name_list[$id_list[$loop+1]],";
                                $loop++;
                            }

                            $header_print = 1;
                            print "\n";
                        }

                        $loop = 0;
                        select BODY;
                        print ",";
                        while ( $loop < ( $item_count - 1 ) )
                        {
                            $cycle_count = $cycle_list[$loop+1] - $cycle_list[$loop];
                            $cycle_count -= $log_overhead;
							if ($cycle_count < 0) {
								$cycle_count = 0;
							}
                            $instruction_count = $inst_list[$loop+1] -  $inst_list[$loop];
                            $instruction_count -= $log_overhead*0.7;
							if ($instruction_count < 0) {
								$instruction_count = 0;
							}
                            $ipc_sum[$loop] += $cycle_count;
                            $inst_sum[$loop] += $instruction_count;
                            #print "$name_list[$id_list[$loop]] -> $name_list[$id_list[$loop+1]], ";
                            #print "cycle:  $cycle_count insturction:  $instruction_count\n";
                            #print "\n";            
                            print $cycle_count;
                            print ", ";
                            $loop++;
                        }
                        print "\n";
                        $entry_count++;
                    }
                    # dump data
                    @cycle_list      = ();
                    @inst_list       = ();
                    @id_list       = ();            
                    $item_count = 0;
                }
            }
        }
    #    foreach $item (@array_tbl)
    #    {
    #        print "$item\n";
    #    }
    #    print "-----\n";
    }

    $_ = $item_list[1];

    if ( /posStr/ )
    {
        $process_name = 1;
		#	print "process_name=$process_name\n";    
    }

    if ( /CP0_Count/ )
    {
        $process_data = 1;
        $process_name = 0;
		#print "process_data=$process_data\n";    
		#print "process_name=$process_name\n";    
    }

}

select RESULT;
$loop = 0;
$total_count = 0;

print "CYCLE,";
while ( $loop < $items - 1 )
{
    $total_count += $ipc_sum[$loop];
    $avg = $ipc_sum[$loop]/$entry_count;
    printf "%d,","$avg";    
    $loop++;
}
print "\n";
$loop = 0;

print "IPC,";
while ( $loop < $items - 1 )
{
	if ( $ipc_sum[$loop] != 0 )
	{
    	$avg = $inst_sum[$loop]/$ipc_sum[$loop];
	}
	else
	{
		$avg = 0;
	}
    printf "%.2f,","$avg";
     $loop++;
}

print "\n";
$loop = 0;

print "%,";
while ( $loop < $items - 1 )
{
    $avg = ($ipc_sum[$loop]*100)/$total_count;
    printf "%.2f,", "$avg";
    $loop++;
}
print "\n";
print ",,\n";

close RESULT;
close HEADER;
close BODY;

system "cat $header_part > $target_file";
system "cat $result_part >> $target_file";
system "cat $body_part >> $target_file";
system "rm $header_part $result_part $body_part";
