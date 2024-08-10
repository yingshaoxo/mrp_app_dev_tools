#!perl -w
#
#
#
# use strict;


sub opendebug
{
    open(DEBUG, ">debug.log");
    select DEBUG;
}

sub closedebug
{
    close(DEBUG);
}

#
#
#
sub processlicfile
{
    my($rherrstruct, $file) = @_;
    my $lastline;
    undef($lastline);
    my($continue, %xline, %errmsg);
    local(%licfile, @lines);
    $continue=0;

    &initerrstruct(\%errmsg);
    @lines=();

    # undef $licfile{"servercount"};
    # undef $licfile{"1"};
    # undef $licfile{"2"};
    # undef $licfile{"3"};
    # undef $licfile{"vendor"};
    # undef $licfile{"lines"};

    $licfile{"lines"} = \@lines;
    $licfile{"servercount"} = 0;

    if(!(open(IN, $file)))
    {        
        &seterrno($rherrstruct, 1);
        &seterrmsg($rherrstruct, "The file $file could not be opened for reading.");
        return \%licfile;
    }
    while($xline=<IN>)
    {
        chomp($xline);
        if ($xline=~/^\s*$/)
        {
            next;
        }
        if($xline eq "")
        {
            next;
        }

        # Remove leading spaces
        $xline =~ s/^ +(\w)/$1/;
        # Remove leading tabs
        $xline =~ s/^\t*//;
        # Remove trailing spaces
        $xline =~ s/ *$//;

        # Comment lines are special, they cannot be continued and
        # another line cannot continue into them
        if($xline =~ /^#/)
        {
            # Purge any outstanding line
            if($continue == 1)
            {
                &processline(\%licfile, \%errmsg, $lastline);
                if(&geterrno(\%errmsg) != 0 )
                {
                    last;
                }
            }
            &processline(\%licfile, \%errmsg, $xline);
            if(&geterrno(\%errmsg) != 0)
            {
                last;
            }
            $continue = 0;
            next;
        }

        # If we have a continued line append the new line to it
        if($continue == 1)
        {
            # Check for a continuing line
            if($xline =~ /\\$/)
            {
                $xline =~ s/\\$//;
                $lastline .= $xline;
            }
            else
            {
                $lastline .= $xline;
                &processline(\%licfile, \%errmsg, $lastline);
                if(&geterrno(\%errmsg) != 0)
                {
                    last;
                }
                $continue = 0;
            }
        }
        else
        {
            # Check for a continuing line
            if($xline =~ /\\$/)
            {
                $xline =~ s/\\$//;
                $lastline = $xline;
                $continue = 1;
            }
            else
            {
                &processline(\%licfile, \%errmsg, $xline);
                if(&geterrno(\%errmsg) != 0)
                {
                    last;
                }
            }
        }
    }
    close(IN);

    return \%licfile;
}

# ##############################################################
# Process a complete license file line
# ##############################################################
sub processline
{
    my $line;
    ($rhlicfile, $rherror, $line) = @_;
    my $rlines;     # Reference to lines entry in licfile

    $rlines = $$rhlicfile{"lines"};
    
    #
    # Process comment lines
    #
    if($line =~ /^#/)
    {
        # Only keep lines that start with '##' as these have
        # been added by a previous running of the license
        # wizard
        if ($line=~/^(##)/)
        {
            push @$rlines, $line;
        }
        return;

    }
    my $keyword;
    $keyword = $line;
    $keyword =~ s/^(\w+).*/$1/;

    $rlines = $$rhlicfile{"lines"};

    if($keyword =~ /^PACKAGE/)
    {
        push @$rlines, $line;
    }
    elsif($keyword =~ /^SERVER/)
    {
        my(@serverbits) = split(/ /, $line);

        $$rhlicfile{"servercount"}++;
        my $count;
        $count = $$rhlicfile{"servercount"};
        $$rhlicfile{$count} = $serverbits[2];
        $$rhlicfile{"server_data_$count"} = $line;

    }
    elsif($keyword =~ /^INCREMENT/)
    {
        my(@featurebits) = split(/ /, $line);
        $date = $featurebits[4];
        if(&checkdate($date))
        {
            push @$rlines, $line;
        }
        else
        {
            # Comment out expired licenses.
            my $newline = '## ' . $line;
            push @$rlines, $newline;
        }
    }
    elsif($keyword =~ /^FEATURE/)
    {
        push @$rlines, $line;
    }
    elsif($keyword =~ /^DAEMON/ or $keyword =~ /^VENDOR/)
    {
        my(@vendorbits) = split(/ /, $line);
        my($bit, $count);
        $$rhlicfile{"vendor"} = $vendorbits[1];
        $$rhlicfile{"vendor_data"} = $line
    }
    elsif($keyword =~ /^USE_SERVER/)
    {
    }
    elsif($keyword =~ /^UPGRADE/)
    {
        push @$rlines, $line;
    }
    else
    {
        &seterrmsg($rherror, "An unknown line type beginning with the word $keyword was found.");
        &seterrno($rherror, 1);
        return;
    }

}

# ######################################################################
# 
# Compare two license file structures and check if they are mergeable
#
# ######################################################################
sub licfilecompare
{
    my($rherrstruct, $st1, $st2) = @_;
    my $matchcount = 0;
    my $mismatchcount = 0;

    if($$st1{"servercount"} != $$st2{"servercount"})
    {
        if(($$st1{"servercount"} == 0)||($$st2{"servercount"} == 0))
        {
            return 1;
        }
        &seterrmsg($rherror, "The files cannot be merged as they contain different numbers of server lines.");
        &seterrno($rherror, 1);
        return 0;
    }
    else
    {
        $matchcount++;
    }

    for($i = 1; $i <= $$st1{"servercount"}; $i++)
    {
        if($$st1{$i} eq $$st2{$i})
        {
            $matchcount++;
        }
        else
        {
            &seterrmsg($rherror, "The server values $$st1{$i} and $$st2{$i} do not match.");
            &seterrno($rherror, 1);
            return 0;
        }
    }
    return 1;
}

# #################################################################
# 
# Merge two license file structures together 
#
# This func assumes everything in the input structures are correct
#
# #################################################################
sub mergefiles
{
    my($rherrstruct, $in1, $in2 ) = @_;
    my($line, $raloclines, $raoutlines);
    my(%masterlist, @masterbits, $masterindex, %masterraw);
    my($master_date, $masternum, $slave_date, $slavenum);
    my(%sublist);
    my($checkline);

    $raSubLines = $$in2{"lines"};
    $raMasterLines = $$in1{"lines"};
  
    #
    # Check to see if both files only have 1 SERVER line each.
    # If they do then check that the data on each line matches
    #
    if (($$in1{"servercount"}==1) && ($$in2{"servercount"}==1))
    {
        my(@mastersvr)=split(/ /, $$in1{"server_data_1"});
        my(@slavesvr)=split(/ /, $$in2{"server_data_1"});
        if (($slavesvr[1] eq $mastersvr[1]) && ($slavesvr[2] eq $mastersvr[2]))
        {
            # Everything OK, continue.
        }
        else
        {
            &seterrno($rherrstruct, 1);
            &seterrmsg($rherrstruct, "The files cannot be merged, as their SERVER lines do not match.");
            return 0;
        }
    }


    #
    # Check to see if both files have 3 SERVER lines each.
    # If they do then check that the data on each line matches
    #
    if (($$in1{"servercount"}==3)&&($$in2{"servercount"}==3))
    {
        my(@mastersvr1)=split(/ /, $$in1{"server_data_1"});
        my(@mastersvr2)=split(/ /, $$in1{"server_data_2"});
        my(@mastersvr3)=split(/ /, $$in1{"server_data_3"});
        my(@slavesvr1)=split(/ /, $$in2{"server_data_1"});
        my(@slavesvr2)=split(/ /, $$in2{"server_data_2"});
        my(@slavesvr3)=split(/ /, $$in2{"server_data_3"});

        my(@mastersort)=sort($mastersvr1[1],
                             $mastersvr1[2],
                             $mastersvr2[1],
                             $mastersvr2[2],
                             $mastersvr3[1],
                             $mastersvr3[2]);
        my(@slavesort)=sort($slavesvr1[1],
                            $slavesvr1[2],
                            $slavesvr2[1],
                            $slavesvr2[2],
                            $slavesvr3[1],
                            $slavesvr3[2]);

        for ($num=0;$num<6;$num++)
        {
            $master=$mastersort[$num];
            $slave=$slavesort[$num];
            if ($slave ne $master)
            {
                &seterrno($rherrstruct, 1);
                &seterrmsg($rherrstruct, "The files cannot be merged, as their SERVER lines do not match.");
                return 0;
            }
        }
    }

    # 
    # Check for the case where the new file has server lines, but the existing
    # file does not.
    if ($$in1{"servercount"}==0)
    {
        if ($$in2{"servercount"}==1)
        {
            $$in1{"servercount"} = $$in2{"servercount"};
            $$in1{"1"} = $$in2{"1"};
            $$in1{"server_data_1"} = $$in2{"server_data_1"};
        }

        if ($$in2{"servercount"}==3)
        {
            $$in1{"servercount"} = $$in2{"servercount"};
            $$in1{"1"} = $$in2{"1"};
            $$in1{"2"} = $$in2{"2"};
            $$in1{"3"} = $$in2{"3"};
            $$in1{"server_data_1"} = $$in2{"server_data_1"};
            $$in1{"server_data_2"} = $$in2{"server_data_2"};
            $$in1{"server_data_3"} = $$in2{"server_data_3"};
            # next;
        }

        # Copy the vendor data across as well.
        if(defined($$in2{"vendor_data"}))
        {
            $$in1{"vendor_data"} = $$in2{"vendor_data"};
        }
    }
    foreach $line (@$raSubLines)
    {
    printf $line."\n";
    }
    printf "=====-----======\n";
    foreach $line (@$raSubLines)
    {
    printf $line."\n";
        #
        # Find out if we have any existing lines which look like the same
        # type as the one we are adding.
        #
        if($line =~ /^#/)
        {
            next;
        }

        $slaveindex = &convert_to_index($line);
        if(&findindexinarray($slaveindex, $raMasterLines))
        {
            @newlines = ();     # Hold new lines to be add
            #
            # Use map to process every line in the master list
            #

            # Signals to add only from the new list
            $usenewlist = 0;
            # Use unmodified existing list
            $useexisting = 0;

            @newmasterlines = map
            {
                my $dollarunderline = $_;
                my($masterindex, $master_date, $slave_date, $masternum, $slavenum);
                $masterindex = &convert_to_index($dollarunderline);
                #
                # Skip PACKAGE lines, comment lines, and
                # lines which don't look similar
                #
                if(($dollarunderline =~ /^PACKAGE/) or
                   ($dollarunderline =~ /^##/) or
                   ($masterindex ne $slaveindex))
                {
                    goto endloop;
                }

                if($dollarunderline eq $line)
                {
                    $usenewarray = 0;
                    $useexisting = 1;
                    goto outloop;
                }
                #
                # Check for license date requirments
                # 
                $master_date=&convert_date($dollarunderline);
                $slave_date=&convert_date($line);
  
                if (($master_date eq "permanent") && ($slave_date eq "permanent"))
                {
                    #
                    # Check entire existing records looking for a match. If there
                    # is one then exit and don't add anything
                    #
                    my($mline);
                    foreach $mline (@$raMasterLines)
                    {
                        if ($line eq $mline)
                        {
                            $useexisting=1;
                            goto outloop;
                        }
                    }

                #
                # If there is a permanent license for a feature, in both files, and the
                # license counts are different then add the license line from the temp
                # file to the master file
                #
                    my($masternum, $slavenum);
                    $masternum=&number_license($dollarunderline);
                    $slavenum=&number_license($line);

                    if ($masternum != $slavenum)
                    {
                        # push @$raMasterLines, $line; 
                        push @newlines , $line; 
                        $usenewlist = 1;
                        goto outloop;
                    }
                    goto endloop;
                }

                if (($master_date eq "permanent") && ($slave_date ne "permanent"))
                {
                    $useexisting = 1;
                    goto outloop;
                }

                if (($master_date ne "permanent") && ($slave_date eq "permanent"))
                {
                    # Add the new slave line
                    push @newlines , $line; 
                    # Comment out the temporary master line
                    $_ = '##' . $dollarunderline;
                    goto endloop;
                }
                else
                {
                    # Got here, got two temporaries

                    # Throw away expired slaves
                    if (&checkdate($slave_date)==0)
                    {
                        $useexisting = 1;
                        goto endloop;
                    }   

                    #
                    # Check to set if the slave line date is later than the
                    # master line date. If it is comment out the master line
                    # and add the slave line
                    #
                    $slavenumdays = &convert_date_to_num($slave_date);
                    $masternumdays = &convert_date_to_num($master_date);
                    if($slavenumdays > $masternumdays)
                    {
                        # Add the new slave line
                        push @newlines , $line; 

                        # Comment out the master line
                        $_ = '##' . $dollarunderline;
                        goto endloop;
                    }
                }

endloop:
                $_;

            } @$raMasterLines;

outloop:
            if($usenewlist == 1)
            {
                (@$raMasterLines) = (@$raMasterLines, @newlines);
            }
            elsif($useexisting == 1)
            {
                # Do nothing, @$raMasterLines is unchanged
            }
            else
            {
                (@$raMasterLines) = (@newmasterlines, @newlines);
            }

        }
        else
        {
            push @$raMasterLines, $line;
        }
    }

    # Some post processing

    foreach $line (@$raMasterLines)
    {

    }
}

# ###############################################################################
#
# Output a license file, using the contents of the input license structure
#
# ###############################################################################
sub outputfile
{
    my($rherrrstruct, $rhlicfile, $outfile) = @_;

    open(OUT, ">$outfile") or die "Failed to open $outfile for output. $!";

    # Redirect prints to OUT
    select OUT;
    
    my $gendate = &getdatestamp();

    print '#' . qq(\n);
    print '# Generated on ' . $gendate . ' by licwizard' . qq(\n);
    print '#' . qq(\n);
    # Print 1 or 3 server lines, if they exist
    my($servercount, $i);
    $servercount = $$rhlicfile{"servercount"};
    if($servercount > 0)
    {
        for($i = 1; $i <= $servercount ; $i++)
        {
            print $$rhlicfile{"server_data_$i"} . qq(\n);
        }

        if(defined($$rhlicfile{"vendor_data"}))
        {
            print $$rhlicfile{"vendor_data"} . qq(\n);
        }

        if($servercount > 0)
        {
            #print "USE_SERVER\n"; # removed to allow future node locked 
                                   # counted licenses.
            print "\n";
        }
    }

    my $rhgroup;
    $rhgroup = $$rhlicfile{"lines"};
    foreach $line (@$rhgroup)
    {
        print $line . qq(\n);
    }
    close(OUT);

    select STDOUT;
}

# ##################################################
# 
# Helper functions for setting/getting error info 
#
# ##################################################
sub initerrstruct
{
    my($rhstruct) = @_;
    $$rhstruct{"errno"} = 0;
    $$rhstruct{"errmsg"} = "";
}

sub seterrno
{
    my($rhstruct, $errno) = @_;
    $$rhstruct{"errno"} = $errno;
}
sub geterrno
{
    my($rhstruct, $errno) = @_;
    return $$rhstruct{"errno"};
}


sub seterrmsg
{
    my($rhstruct, $errmsg) = @_;
    $$rhstruct{"errmsg"} = $errmsg;
}
sub geterrmsg
{
    my($rhstruct, $errmsg) = @_;
    return $$rhstruct{"errmsg"};
}

# #################################################################
#                                                                 #
# Check a FLEXlm expiry date to make sure that it has not expired #
#                                                                 #
# #################################################################

sub checkdate($)
{
    my $indate = $_[0];
    
    if($indate =~ /-0$/)
    {
        return 1;
    }
    if($indate =~ /-00$/)
    {
        return 1;
    }
    if($indate =~ /-0000$/)
    {
        return 1;
    }
    if($indate eq 'permanent')
    {
        return 1;
    }
    # All 2 digit dates, which are not 00, must refer to 19xx
    # expiry date.  Therefore they have expired and should be
    # excluded, from the output license.
    if($indate =~ /-(\d\d)$/)
    {
        return 0;
    }

    my($fileday, $filemonth, $fileyear, %monthmap);
    $indate =~ m/(\d{1,2})-(\w\w\w)-(\d{1,4})/;
    $fileday = $1;
    $filemonth = $2;
    $fileyear = $3;

    %monthmap = ('jan'=>0, 'feb'=>1, 'mar'=>2, 'apr'=>3, 'may'=>4,
             'jun'=>5, 'jul'=>6, 'aug'=>7, 'sep'=>8, 'oct'=>9,
             'nov'=>10, 'dec'=>11);
    $filemonth = $monthmap{$filemonth};
    $fileyear = $fileyear - 1900;

    my($sec, $min, $hour, $mday, $mon, $year, $wday, $yday, $isdst) =
        localtime(time);

    # Discard years earlier than the current year
    if($fileyear < $year)
    {
        return 0;
    }
    # Discard years earlier than the current year
    if($fileyear > $year)
    {
        return 1;
    }

    # At this point we now only a date which has a year equal to
    # the current system year

    if(($filemonth < $mon) or
       (($filemonth == $mon) and ($fileday < $mday)))
    {
        return 0;
    }
    1;
}

sub getdatestamp
{
    my($sec, $min, $hour, $mday, $mon, $year, $wday, $yday, $isdst) =
        localtime(time);
    my($date, $curryear, $currmonth, $currday);

    $currmonth =('jan', 'feb', 'mar', 'apr', 'may', 'jun', 
                 'jul', 'aug', 'sep', 'oct', 'nov', 'dec')[$mon];
    $curryear = $year + 1900;
    $date = $curryear . '-' . $currmonth . '-' . $mday;

    return $date;
}

# #################################################################
#                                                                 #
# Check the date to see if it is a permanent license              #
#                                                                 #
# Return Value                                                    #
# = 1  -  Permanent License                                       #
# = 0  -  Dated license                                           #
#                                                                 #
# #################################################################

sub ispermlicense($)
{
    my $indate = $_[0];
    
    if($indate =~ /-0$/)
    {
        return 1;
    }
    if($indate =~ /-00$/)
    {
        return 1;
    }
    if($indate =~ /-0000$/)
    {
        return 1;
    }
    if($indate eq 'permanent')
    {
        return 1;
    }

    return 0;
}

# #################################################################
#                                                                 #
# Convert the values from a line into an index value              #
#                                                                 #
# Return Value                                                    #
#   Index value string                                            #
#                                                                 #
# #################################################################

sub convert_to_index($)
{
    my($line) = @_;
    my(@masterbits, $linetype, $version );
    local($masterindex);

    @masterbits = split(/ /, $line);

    $linetype = $masterbits[0];
    if($linetype eq 'FEATURE')
    {
        $linetype = 'INCREMENT';
    }

    if($linetype eq 'INCREMENT')
    {
        #
        # Convert version into a number which does not have any extraneous zeroes
        # after the decimal point
        #
        $version = $masterbits[3] + 1 - 1;

        $masterindex = $linetype . '_' . $masterbits[1] . '_' . $version;
        $masterindex =~ s/\./_/g;
        return $masterindex;
    }
    elsif($linetype eq 'PACKAGE')
    {
        $version = $masterbits[3] + 1 - 1;
        $masterindex = $linetype . '_' . $masterbits[1] . '_' . $version;
        $masterindex =~ s/\./_/g;
        return $masterindex
    }
    else
    {
        return undef;
    }
    return undef;
}

sub convert_date($)
{
    my($line)=@_;
    my(@masterbits);

    @masterbits=split(/ /,$line);

    #
    # If its a non-expiring license convert it to the text '_permanent'
    #
    if(&ispermlicense($masterbits[4]))
    {
        return "permanent";
    }
    else
    {
        return $masterbits[4];
    }
}

sub convert_date_to_num
{
    my($indate)=@_;
    #
    # If its a non-expiring license convert it to the text '_permanent'
    #
    if(&ispermlicense($indate))
    {
        return 0;
    }
    else
    {
        my($fileday, $filemonth, $fileyear, %monthmap, $date);
        $indate = $indate;
        $indate =~ m/(\d{1,2})-(\w\w\w)-(\d{1,4})/;
        $fileday = $1;
        $filemonth = $2;
        $fileyear = $3;

        %monthmap = ('jan'=>0, 'feb'=>1, 'mar'=>2, 'apr'=>3, 'may'=>4,
             'jun'=>5, 'jul'=>6, 'aug'=>7, 'sep'=>8, 'oct'=>9,
             'nov'=>10, 'dec'=>11);
        $filemonth = $monthmap{$filemonth};
        #
        # Convert up to somehting we can compare
        # 
        $date = ($fileyear * 366) + ($filemonth * 31) + $fileday;
        return $date;
    }
}

sub number_license($)
{
    my($line)=@_;
    my(@masterbits)=split(/ /,$line);
    #
    # Check for an uncounted license and convert it to the text '_uncounted'
    #
    if(($masterbits[5] =~ /^uncounted/) or ($masterbits[5] eq '0'))
    {
        return 'uncounted';
    }
    else
    {
        return $masterbits[5] ;
    }
    return 0;
}

# #################################################################
#                                                                 #
# Convert the values from a line into a feature_ver strin         #
#                                                                 #
# Return Value                                                    #
#   Index value string                                            #
#                                                                 #
# #################################################################

sub convert_to_feature_ver_string($)
{
    my($line) = @_;
    my(@masterbits, $linetype, $version );
    local($masterindex);

    @masterbits = split(/ /, $line);

    #
    # Convert version into a number which does not have any extraneous zeroes
    # after the decimal point
    #
    $version = $masterbits[3] + 1 - 1;

    $masterindex = $masterbits[1] . '_' . $version;
    $masterindex =~ s/\./_/g;
    return $masterindex;
}

sub makedirect($)
{
    my($directory)=@_;
    my (@array,$count,$makey);
    @array = split(/\\/, $directory);

    $count=1;
    $makey=$array[0];

    foreach (@array)
    {
            $makey=$makey."\\".$array[$count];
            mkdir ($makey,0);
            $count++;
    }
}

#
# Find a matching index value in the line array
#
sub findindexinarray
{
    my($searchfor, $thearray) = @_;
    my $index;
    
    foreach $word (@$thearray)
    {
        next if($word =~ /^#/);
        $index = &convert_to_index($word);

        if($index eq $searchfor)
        {
            return 1;
        }
    }
    0;
}

1;
